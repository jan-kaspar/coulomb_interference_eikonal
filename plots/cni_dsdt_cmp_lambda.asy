import root;
import pad_layout;
include "common_code.asy";

string topDir = "../";

string f_eikonal = topDir + "test_cni_eikonal.root";

ySizeDef = 5cm;

xTicksDef = LeftTicks(0.005, 0.001);

//----------------------------------------------------------------------------------------------------

for (int mi : models.keys)
{
	NewPad(false);
	label(m_labels[mi]);
}

NewRow();

for (int mi : models.keys)
{
	NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t\ung{mb/GeV^2}$");
	//scale(Linear, Log);

	for (int lai : lambdas.keys)
	{
		string label = l_labels[lai];
		if (label == lambda_ref)
			label += " [ref]";

		pen p = StdPen(lai + 1);

		draw(RootGetObject(f_eikonal, models[mi] + "/" + formFactor + "/la=" + lambdas[lai] + "/CH/g_dsdt"), p, label);
	}

	limits((0, 300), (0.02, 1000), Crop);
}

AttachLegend();

NewRow();

for (int mi : models.keys)
{
	NewPad("$|t|\ung{GeV^2}$", "$(\d\sigma/\d t - \hbox{ref}) / \hbox{ref}$");
	//scale(Linear, Log);

	for (int lai : lambdas.keys)
	{
		pen p = StdPen(lai + 1);

		RootObject obj = RootGetObject(f_eikonal, models[mi] + "/" + formFactor + "/la=" + lambdas[lai] + "/CH/g_dsdt");
		RootObject obj_ref = RootGetObject(f_eikonal, models[mi] + "/" + formFactor + "/la=" + lambda_ref + "/CH/g_dsdt");

		DrawRel(obj, obj_ref, p);
	}

	limits((0, -0.001), (0.02, 0.001), Crop);
}

GShipout(vSkip=0mm);
