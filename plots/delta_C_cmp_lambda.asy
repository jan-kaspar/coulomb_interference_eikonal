import root;
import pad_layout;
include "common_code.asy";

string topDir = "../";

string f_eikonal = topDir + "test_cni_eikonal.root";

string model = "exp3_con";	// doesn't matter

ySizeDef = 5cm;

xTicksDef = LeftTicks(5., 1.);

//----------------------------------------------------------------------------------------------------

NewPad("$b\ung{GeV^{-2}}$", "$\de^{\rm C}(b)$");

for (int lai : lambdas.keys)
{
	string label = l_labels[lai];
	//if (label == lambda_ref)
	//	label += " [ref]";

	pen p = StdPen(lai + 1);

	draw(RootGetObject(f_eikonal, model + "/" + formFactor + "/la=" + lambdas[lai] + "/de_C_re"), p, label);
	draw(RootGetObject(f_eikonal, model + "/none/la=" + lambdas[lai] + "/de_C_re"), p+dashed);
}

limits((0, -0.2), (20., 0.), Crop);

AttachLegend();

NewRow();

NewPad("$b\ung{GeV^{-2}}$", "$(\de^{\rm C} - \hbox{ref}) / \hbox{ref}$");

for (int lai : lambdas.keys)
{
	pen p = StdPen(lai + 1);

	RootObject g = RootGetObject(f_eikonal, model + "/" + formFactor + "/la=" + lambdas[lai] + "/de_C_re");
	RootObject g_ref = RootGetObject(f_eikonal, model + "/none/la=" + lambdas[lai] + "/de_C_re");

	DrawRel(g, g_ref, p);
}

limits((0, -0.001), (20., 0.001), Crop);


GShipout(vSkip=0mm);
