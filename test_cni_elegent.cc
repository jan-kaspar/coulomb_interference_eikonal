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

	for (double mt = 1E-4; mt <= 0.2;)
	{
		TComplex A = 0;
		
		if (formula == "trivial")
			A = model->Amp(-mt) + coulomb->Amp_pure(-mt);
		
		if (formula == "KL")
			A = coulomb->Amp_KL(-mt);

		int idx = g_dsdt->GetN();
		g_dsdt->SetPoint(idx, mt, cnts->sig_fac * A.Rho2());
		g_phase->SetPoint(idx, mt, A.Theta());

		double dmt = 1E-4;
		if (mt > 3E-3)
			dmt = 1E-3;
		if (mt > 0.1)
			dmt = 1E-2;

		mt += dmt;
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
		"trivial",
		"KL",
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
			gDirectory = d_formula;

			PlotOne(formula);
		}
	}

	// clean up
	delete f_out;

	return 0;
}
