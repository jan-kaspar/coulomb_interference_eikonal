#include "include/Elegent/Constants.h"
#include "include/Elegent/CoulombInterference.h"

#include "HadronicFitModel.h"
#include "HadronicModels.h"

#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>

#include "TFile.h"
#include "TGraph.h"

#include "Interpolator.h"

using namespace std;

gsl_integration_workspace *gsl_w;
unsigned long gsl_w_size = 4000000;

double la;

double precision = 1E-5;

//----------------------------------------------------------------------------------------------------
// COULOMB EIKONAL

double F_C_born_input(double mt)
{
	return - cnts->s * cnts->alpha * coulomb->FF_sq(-mt) / (mt + la*la);
}

double delta_C_integ(double q, void *pars)
{
	double *dpars = (double *) pars;
	double b = dpars[0];
	return q * TMath::BesselJ0(b*q) * F_C_born_input(q*q);
}

double delta_C_exact(double b)
{
  	double result, error;
	gsl_function F;
  	F.function = &delta_C_integ;
  	F.params = &b;

	// TODO
	//const double q_max = pow(10., max(3., -0.75*log10(b) + 3.));
	const double q_max = pow(10., max(3., -log10(b) + 3.));

	// TODO
	//const double K = 1E-7;
	//const double q_max = (b < K) ? 1./K : 1./pow(K * K * b, 1./3.);

	//                            fcn, from, to,  abserr,relerr, limit,   key,             workspace, 
  	//int status = 
	gsl_integration_qag(&F, 0., q_max, 0., 1E-13, gsl_w_size, GSL_INTEG_GAUSS61, gsl_w, &result, &error);
	//printf("\tb = %.1E | status = %i, intervals = %li, val = %.1E +- %.1E\n", b, status, gsl_w->size, result, error);

	return result / cnts->s;
}

Interpolator *delta_C_interpolator;

double delta_C_intp(double b)
{
	return delta_C_interpolator->EvalRe(b);
}

double delta_C_asympt(double b)
{
	return -cnts->alpha * TMath::BesselK0(la*b);
}

double delta_C(double b)
{
	if (b > 2E1)
		return delta_C_asympt(b);

	return delta_C_intp(b);
}

//----------------------------------------------------------------------------------------------------
// HADRONIC EIKONAL

TComplex delta_H_input(double b)
{
	const TComplex A = 2. * cnts->p_cms / cnts->sqrt_s * model->Prf(b * cnts->hbarc);
	return TComplex::Log(1. + 2. * i * A) / 2. / i;
}

Interpolator *delta_H_interpolator;
TComplex delta_H(double b)
{
	return delta_H_interpolator->Eval(b);
}

double delta_H_re(double b)
{
	return delta_H(b).Re();
}

double delta_H_im(double b)
{
	return delta_H(b).Im();
}

//----------------------------------------------------------------------------------------------------
// AMPLITUDE CALCULATION

enum ModeType { mC, mH, mCH };
unsigned int mode;

string GetModeName()
{
	switch (mode)
	{
		case mC:
			return "C";
		case mH:
			return "H";
		case mCH:
			return "CH";
		default:
			return "unknown";
	}
}

TComplex delta(double b)
{
	switch (mode)
	{
		case mC:
			return delta_C(b);
		case mH:
			return delta_H(b);
		case mCH:
			return delta_C(b) + delta_H(b);
		default:
			return 0;
	}
}

TComplex prof(double b)
{
	// optimisation: whenever Coulomb is inluded, subtract the Born term from delta
	// and add it directly amplitude

	if (mode == mH)
		return TComplex::Exp(2.*i*delta(b)) - 1.;
	else
		return TComplex::Exp(2.*i*delta(b)) - 1. - 2.*i*delta_C(b);
}

double b_integ_re(double b, void *p)
{
	double *pars = (double *) p;
	double q = sqrt(- pars[0]);
  	TComplex res = b * TMath::BesselJ0(b*q) * prof(b);
	return res.Re();
}

double b_integ_im(double b, void *p)
{
	double *pars = (double *) p;
	double q = sqrt(-pars[0]);
  	TComplex res = b * TMath::BesselJ0(b*q) * prof(b);
	return res.Im();
}

TComplex Amp(double mt)
{
	double t = -mt;
	//printf("\tt=%.2E, precision=%.1E\n", t, precision);

  	double result_re, error_re;
	gsl_function F_re;
  	F_re.function = &b_integ_re;
  	F_re.params = &t;

	const double b_max = max(100., 10./la);

	//                 fcn, from, to,   abserr,  relerr,  wkspace size,   iteg. rule,  workspace, 
	gsl_integration_qag(&F_re, 0., b_max, 0., precision, gsl_w_size, GSL_INTEG_GAUSS61, gsl_w, &result_re, &error_re); 
  	
	double result_im, error_im;
	gsl_function F_im;
  	F_im.function = &b_integ_im;
  	F_im.params = &t;

	gsl_integration_qag(&F_im, 0., b_max, 0., precision, gsl_w_size, GSL_INTEG_GAUSS61, gsl_w, &result_im, &error_im); 

	TComplex amp(result_re, result_im);
	amp *= cnts->s / 2. / i;

	// optimisation: whenever Coulomb is inluded, subtract the Born term from delta
	// and add it directly to amplitude
	if (mode != mH)
		amp += F_C_born_input(mt);

	//printf("\tAMP: re = %.1E, im = %.1E, mod = %.1E, arg = %.1f\n", amp.Re(), amp.Im(), amp.Rho(), amp.Theta());

	return amp;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main()
{
	// init
	gsl_w  = gsl_integration_workspace_alloc(gsl_w_size);
	gsl_set_error_handler_off();

	Constants::Init(2*4000., cnts->mPP);
    cnts->Print();

	coulomb->ffType = coulomb->ffPuckett;
	coulomb->Print();

	InitHadronicModels();

	vector<CoulombInterference::FFType> formFactors = {
		CoulombInterference::ffPuckett,
		CoulombInterference::ffNone
	};

	vector<Model *> models = {
		m_exp3_con,
		m_exp3_per
	};

	vector<double> lambdas = {
		1E-2,
		1E-3,
		1E-4,
		3E-5,
		1E-5,
	};
	
	// prepare output
	TFile *f_out = new TFile("test_cni_eikonal.root", "recreate");

	// sampling
	for (Model *m : models)
	{
		model = m;

		TDirectory *d_model = f_out->mkdir(model->shortLabel.name.c_str());
		gDirectory = d_model;

		printf("model = %s\n", model->shortLabel.name.c_str());

		printf("\tsampling delta_H\n");
		delta_H_interpolator = new Interpolator(20001, 0., 100., false);
		delta_H_interpolator->Sample(delta_H_input);
		delta_H_interpolator->GraphRe("de_H_re")->Write();
		delta_H_interpolator->GraphIm("de_H_im")->Write();

		for (const auto &formFactor : formFactors)
		{
			coulomb->ffType = formFactor;
			const auto &ffName = coulomb->GetFFName();

			TDirectory *d_ff = d_model->mkdir(ffName.c_str());

			printf("\tform factor %s\n", ffName.c_str());

			for (const auto &lambda : lambdas)
			{
				la = lambda;

				char buf[100];
				sprintf(buf, "la=%.1E", la);
				TDirectory *d_lambda = d_ff->mkdir(buf);
				gDirectory = d_lambda;

				printf("\t\t%s\n", buf);
				
				precision = 1E-8;

				for (const double &b : {1E-8, 1E-4, 1E-2, 1., 10.})
				{
					sprintf(buf, "de_C_integ_b=%.1E", b);
					TGraph *g_de_C_integ = new TGraph();
					g_de_C_integ->SetName(buf);
					g_de_C_integ->SetTitle(";q   (GeV)");

					double pars[1] = {b};

					for (double q = 1E-8; q < 1E4; q += 10.)
					{
						double v = delta_C_integ(q, pars);

						int idx = g_de_C_integ->GetN();
						g_de_C_integ->SetPoint(idx, q, v);
					}

					g_de_C_integ->Write();
				}

				printf("\t\t\tsampling delta_C\n");
				delta_C_interpolator = new Interpolator(5000, 1E-8, 21., true);
				delta_C_interpolator->Sample(delta_C_exact);
				delta_C_interpolator->GraphRe("de_C_re")->Write();
				delta_C_interpolator->GraphIm("de_C_im")->Write();

				// go through C, H and CH amplitudes
				for (const auto &md : { mC, mH, mCH })
				{
					if (formFactor == CoulombInterference::ffNone && md != mC)
						continue;

					mode = md;

					printf("\t\t\tamplitude: %s\n", GetModeName().c_str());
				
					TDirectory *d_mode = d_lambda->mkdir(GetModeName().c_str());
					gDirectory = d_mode;

					TGraph *g_de_mod = new TGraph();
					g_de_mod->SetName("g_de_mod");

					for (double b = 0.; b <= 1000.; b += 1.)
					{
						TComplex P = prof(b);

						int idx = g_de_mod->GetN();
						g_de_mod->SetPoint(idx, b, P.Rho());
					}

					g_de_mod->Write();

		
					TGraph *g_amp_re = new TGraph();
					g_amp_re->SetName("g_amp_re");
					
					TGraph *g_amp_im = new TGraph();
					g_amp_im->SetName("g_amp_im");

					TGraph *g_dsdt = new TGraph();
					g_dsdt->SetName("g_dsdt");

					TGraph *g_phase = new TGraph();
					g_phase->SetName("g_phase");
					
					const double t_min = 1E-4;
					const double t_max = 1.5;
					const int n = 300;

					const double xi = pow(t_max/t_min, 1./(n-1));

					double mt = t_min;
					for (unsigned int i = 0; i < n; i++, mt *= xi)
					{
						TComplex amp = Amp(mt);
						g_amp_re->SetPoint(i, mt, amp.Re());
						g_amp_im->SetPoint(i, mt, amp.Im());
						g_dsdt->SetPoint(i, mt, cnts->sig_fac * amp.Rho2());
						g_phase->SetPoint(i, mt, amp.Theta());
					}
			
					g_amp_re->Write();
					g_amp_im->Write();
					g_dsdt->Write();
					g_phase->Write();
				}
			}
		}
	}

	// clean up
	gsl_integration_workspace_free(gsl_w);

	delete f_out;

	return 0;
}
