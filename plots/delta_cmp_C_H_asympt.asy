import root;
import pad_layout;
include "common_code.asy";

string topDir = "../";

string f_eikonal = topDir + "test_cni_eikonal.root";

string lambda = "1.0E-03";

//ySizeDef = 5cm;

xTicksDef = LeftTicks(5., 1.);

//----------------------------------------------------------------------------------------------------

void DrawOne(string model)
{
	NewPad("$b\ung{GeV^{-1}}$", "$\de(b)$");

	TGraph_reducePoints = 100;

	draw(RootGetObject(f_eikonal, model + "/de_H_re"), black, "$\Re \de^{\rm H}$");
	draw(RootGetObject(f_eikonal, model + "/de_H_im"), blue, "$\Im \de^{\rm H}$");

	TGraph_reducePoints = 1;
	draw(RootGetObject(f_eikonal, model + "/" + formFactor + "/la=" + lambda + "/de_C_asympt"), heavygreen, "$\de^{\rm C}_{\rm asym}$");

	//TGraph_reducePoints = 100;
	draw(RootGetObject(f_eikonal, model + "/" + formFactor + "/la=" + lambda + "/de_C_re"), red + dashed, "$\de^{\rm C}$");
}

//----------------------------------------------------------------------------------------------------

for (int mi : models.keys)
{
	string model = models[mi];

	NewRow();

	NewPad(false);
	label(m_labels[mi]);

	DrawOne(model);
	limits((0, -0.1), (50, +0.5), Crop);

	DrawOne(model);
	limits((50, -0.03), (100, +0.01), Crop);

	AttachLegend(NW, NE);
}

GShipout(vSkip=0mm);
