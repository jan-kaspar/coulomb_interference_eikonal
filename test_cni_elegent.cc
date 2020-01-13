#include "include/Elegent/Constants.h"
#include "include/Elegent/CoulombInterference.h"

#include "HadronicFitModel.h"
#include "HadronicModels.h"

#include "TGraph.h"
#include "TFile.h"

#include <vector>

using namespace std;
using namespace Elegent;

//----------------------------------------------------------------------------------------------------

void PlotOne(const string &formula)
{
	TGraph *g_dsdt = new TGraph(); g_dsdt->SetName("g_dsdt");
	TGraph *g_phase = new TGraph(); g_phase->SetName("g_phase");

	const double t_min = 1E-4;
	const double t_max = 1.5;
	const int n = 300;

	const double xi = pow(t_max/t_min, 1./(n-1));

	double mt = t_min;
	for (unsigned int i = 0; i < n; i++, mt *= xi)
	{
		TComplex A = 0;
		
		if (formula == "coulomb")
			A = coulomb->Amp_pure(-mt);

		if (formula == "trivial")
			A = model->Amp(-mt) + coulomb->Amp_pure(-mt);

		if (formula == "SWY")
			A = coulomb->Amp_SWY(-mt);
		
		if (formula == "Cahn")
			A = coulomb->Amp_Cahn(-mt);

		if (formula == "KL")
			A = coulomb->Amp_KL(-mt);

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
		"coulomb",
		"trivial",
		"SWY",
		"Cahn",
		"KL",
	};

	vector<CoulombInterference::FFType> formFactors = {
		CoulombInterference::ffNone,
		CoulombInterference::ffPuckett,
	};

	// prepare output
	TFile *f_out = new TFile("test_cni_elegent.root", "recreate");

	for (Model *m : models)
	{
		model = m;

		TDirectory *d_model = f_out->mkdir(model->shortLabel.name.c_str());

		for (const auto formula : formulae)
		{
			TDirectory *d_formula = d_model->mkdir(formula.c_str());

			for (const auto &ff: formFactors)
			{
				if (formula != "coulomb" && ff == CoulombInterference::ffNone)
					continue;

				coulomb->ffType = ff;

				TDirectory *d_ff = d_formula->mkdir(coulomb->GetFFName().c_str());
				gDirectory = d_ff;

				PlotOne(formula);
			}
		}
	}

	// clean up
	delete f_out;

	return 0;
}
