#include "include/Elegent/Constants.h"
#include "include/Elegent/CoulombInterference.h"

#include "HadronicFitModel.h"
#include "HadronicModels.h"

#include "Interpolator.h"

#include "TGraph.h"
#include "TFile.h"

#include <vector>

using namespace std;
using namespace Elegent;

//----------------------------------------------------------------------------------------------------

unsigned long integ_workspace_size;
gsl_integration_workspace *integ_workspace;

//--------------------------------------------------------------------------------------------------

double delta_C_hat_source_integrand(double q, double *par, const void*)
{
	const double &b = par[0];

	const double t = - q*q * cnts->p_cms * cnts->p_cms;

	return coulomb->FF_sq(t) / q * ( TMath::BesselJ0(b*q) - 1.);
}

double delta_C_hat_source(double b)
{
	double par[] = { b };
	
	const double t_max = 500.;
	const double q_min = 1E-9;
	const double q_max = sqrt(t_max) / cnts->p_cms;
	const double rel_precision = 1E-5;

	const double I = RealIntegrate(delta_C_hat_source_integrand, par, NULL, q_min, q_max, 0., rel_precision,
		integ_workspace_size, integ_workspace, "delta_C_hat_source");

	return - cnts->alpha * I;
}

Interpolator *delta_C_hat_interpolator;

//--------------------------------------------------------------------------------------------------

TComplex Xi_integrand(double b, double *par, const void*)
{
	const double &q = par[0];

	return b * TMath::BesselJ0(b*q) * TComplex::Exp(2. * i * delta_C_hat_interpolator->EvalRe(b));
}

TComplex Xi_source(double q)
{
	double par[] = { q };
	
	const double b_min = 0.;
	const double b_max = 500.;
	const double rel_precision = 1E-5;

	const TComplex I = ComplexIntegrate(Xi_integrand, par, NULL, b_min, b_max, 0., rel_precision,
		integ_workspace_size, integ_workspace, "Xi_source");

	return 2. * M_PI * I;
}

Interpolator *Xi_interpolator;

//--------------------------------------------------------------------------------------------------

TComplex Amp_CH_Petrov13(double /*t*/)
{
	// TODO
	return 0.;
}

//--------------------------------------------------------------------------------------------------

TComplex Petrov17_integrand(double tp, double *par, const void *)
{
	const double &t = par[0];
	const TComplex T_hadron_t(par[1], par[2]);

	const double I = coulomb->I_function(t, tp);
	
	TComplex a = model->Amp(tp) / T_hadron_t;
	return (a - 1.) * I	/ 2. / cnts->pi;
}

//----------------------------------------------------------------------------------------------------

TComplex Amp_CH_Petrov17(double t)
{
	/**
	Function B_integrand(t', t) has problems at point t' = t. It is not defined there (left and right limits are different),
	it is not continuous at the point. To avoid problems, we cut out a small interval (t-tau, t+tau), tau > 0 from the
	the integration region (t_min, 0). The second note concerns exponential fall off of B_integrand as one goes with t' away
	from t. Thus one can take (t-T, 0) instead of (t_min, 0). Of course, T must be suffciently large.
	
	In fact, one must be careful with lower bound t+tau, since it must be less than 0. Otherwise contribution from the 
	region (t+tau, 0) isn't present.
	**/

	const TComplex amp_t = model->Amp(t);
	double par[] = { t, amp_t.Re(), amp_t.Im() }; 

	const double T = 10.;
	const double tau = 1E-10;
	const double precision = 1E-2;

	TComplex Psi = ComplexIntegrate(Petrov17_integrand, par, NULL, t - T, t - tau, 1E-7, precision,
		integ_workspace_size, integ_workspace, "Amp_CH_Petrov17/ht");

	if (t + tau < 0.)
		Psi += ComplexIntegrate(Petrov17_integrand, par, NULL, t + tau, 0., 1E-7, precision,
			integ_workspace_size, integ_workspace, "Amp_CH_Petrov17/lt");

	/*
	printf("    Psi.Re = %.2E, Psi.Im = %.2E\n", Psi.Re(), Psi.Im());
	TComplex Psi_check = coulomb->B_term(t);
	printf("    check: Psi.Re = %.2E, Psi.Im = %.2E\n", Psi_check.Re(), Psi_check.Im());
	*/

	//Psi -= coulomb->A_term(t);

	return coulomb->Amp_pure(t) + amp_t * (1. + i * cnts->alpha * Psi);
}

//----------------------------------------------------------------------------------------------------

void PlotOne(const string &formula)
{
	TGraph *g_dsdt = new TGraph(); g_dsdt->SetName("g_dsdt");
	TGraph *g_phase = new TGraph(); g_phase->SetName("g_phase");

	const double t_min = 1E-4;
	const double t_max = 1.5;

	// TODO
	const int n = 300;
	//const int n = 30;

	const double xi = pow(t_max/t_min, 1./(n-1));

	double mt = t_min;
	for (unsigned int i = 0; i < n; i++, mt *= xi)
	{
		//printf("t = %.2E\n", mt);

		TComplex A = 0;
		
		if (formula == "petrov_13")
			A = Amp_CH_Petrov13(-mt);
		
		if (formula == "petrov_17")
			A = Amp_CH_Petrov17(-mt);

		int idx = g_dsdt->GetN();
		g_dsdt->SetPoint(idx, mt, cnts->sig_fac * A.Rho2());
		g_phase->SetPoint(idx, mt, A.Theta());
	}

	g_dsdt->Write();
	g_phase->Write();
}

//----------------------------------------------------------------------------------------------------

int main()
{
	Constants::Init(2*4000, cnts->mPP);
	cnts->Print();

	coulomb->ffType = coulomb->ffPuckett;
	coulomb->Print();

	InitHadronicModels();

	vector<Model *> models = {
		m_exp3_con,
		m_exp3_per
	};

	vector<string> formulae = {
		"petrov_17",
	};

	// allocate memory
	integ_workspace_size = 10000;
	integ_workspace = gsl_integration_workspace_alloc(integ_workspace_size);

	// prepare output
	TFile *f_out = new TFile("test_cni_petrov.root", "recreate");

	// prepare calculations
	printf("* sampling delta_C_hat\n");
	delta_C_hat_interpolator = new Interpolator(1000, 0., 500., false);
	delta_C_hat_interpolator->Sample(delta_C_hat_source);
	delta_C_hat_interpolator->GraphRe("detla_C_hat.re")->Write();

	printf("* sampling Xi\n");
	Xi_interpolator = new Interpolator(1000, 0., 0.5, false);
	Xi_interpolator->Sample(Xi_source);
	Xi_interpolator->GraphRe("Xi.re")->Write();
	Xi_interpolator->GraphIm("Xi.im")->Write();

	for (Model *m : models)
	{
		model = m;

		TDirectory *d_model = f_out->mkdir(model->shortLabel.name.c_str());

		printf("* model: %s\n", model->shortLabel.name.c_str());

		for (const auto formula : formulae)
		{
			TDirectory *d_formula = d_model->mkdir(formula.c_str());
			gDirectory = d_formula;

			printf("    formula: %s\n", formula.c_str());

			PlotOne(formula);
		}
	}

	// clean up
	delete f_out;

	return 0;
}
