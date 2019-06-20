#include "include/Elegent/Constants.h"
#include "include/Elegent/CoulombInterference.h"

#include "include/Elegent/PPPModel.h"

#include "TGraph.h"
#include "TFile.h"

#include <vector>

using namespace std;
using namespace Elegent;

//----------------------------------------------------------------------------------------------------

void PlotOne()
{
	TGraph *g_G1_re = new TGraph(); g_G1_re->SetName("g_G1_re");
	TGraph *g_G1_im = new TGraph(); g_G1_im->SetName("g_G1_im");

	TGraph *g_G2_re = new TGraph(); g_G2_re->SetName("g_G2_re");
	TGraph *g_G2_im = new TGraph(); g_G2_im->SetName("g_G2_im");

	TGraph *g_G3_re = new TGraph(); g_G3_re->SetName("g_G3_re");
	TGraph *g_G3_im = new TGraph(); g_G3_im->SetName("g_G3_im");

	TGraph *g_G_all_re = new TGraph(); g_G_all_re->SetName("g_G_all_re");
	TGraph *g_G_all_im = new TGraph(); g_G_all_im->SetName("g_G_all_im");

	for (double mt = 1E-3; mt <= 0.3;)
	{
		TComplex G1 = + coulomb->A_term(-mt);
		TComplex G2 = - coulomb->B_term(-mt);
		TComplex G3 = - coulomb->C_term(-mt);

		int idx = g_G1_re->GetN();
		g_G1_re->SetPoint(idx, mt, G1.Re());
		g_G1_im->SetPoint(idx, mt, G1.Im());

		g_G2_re->SetPoint(idx, mt, G2.Re());
		g_G2_im->SetPoint(idx, mt, G2.Im());

		g_G3_re->SetPoint(idx, mt, G3.Re());
		g_G3_im->SetPoint(idx, mt, G3.Im());

		g_G_all_re->SetPoint(idx, mt, (G1 + G2 + G3).Re());
		g_G_all_im->SetPoint(idx, mt, (G1 + G2 + G3).Im());

		double dmt = 1E-3;
		if (mt > 0.1)
			dmt = 1E-2;

		mt += dmt;
	}

	g_G1_re->Write();
	g_G1_im->Write();

	g_G2_re->Write();
	g_G2_im->Write();

	g_G3_re->Write();
	g_G3_im->Write();

	g_G_all_re->Write();
	g_G_all_im->Write();
}

//----------------------------------------------------------------------------------------------------

int main()
{
	Constants::Init(2*6500, cnts->mPP);
    cnts->Print();

	coulomb->ffType = coulomb->ffPuckett;
	//coulomb->ffType = coulomb->ffNone;

	PPPModel *ppp3 = new PPPModel();
	ppp3->Configure(PPPModel::v3P);
	ppp3->Init();

	model = ppp3;

	// prepare output
	TFile *f_out = new TFile("test_cni_terms.root", "recreate");

	for (double T : { 3., 10., 30.})
	{
		char buf[100];
		sprintf(buf, "T=%.1f", T);
		gDirectory = f_out->mkdir(buf);

		coulomb->T = T;
		coulomb->Print();

		PlotOne();
	}

	// clean up
	delete f_out;

	return 0;
}
