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

void PlotOne()
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
		TComplex A = model->Amp(-mt);

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
	Constants::Init(2*4000., cnts->mPP);
	cnts->Print();

	coulomb->ffType = coulomb->ffPuckett;
	coulomb->Print();

	InitHadronicModels();

	vector<Model *> models = {
		m_exp3_con,
		m_exp3_per
	};

	// prepare output
	TFile *f_out = new TFile("test_hadronic.root", "recreate");

	for (Model *m : models)
	{
		model = m;

		gDirectory = f_out->mkdir(model->shortLabel.name.c_str());

		PlotOne();
	}

	// clean up
	delete f_out;

	return 0;
}
