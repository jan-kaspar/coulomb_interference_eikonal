import root;
import pad_layout;
include "common_code.asy";

string topDir = "../";

string f_eikonal = topDir + "test_cni_eikonal.root";

string f_files[], f_objs[], f_labels[];
pen f_pens[];

f_files.push("test_cni_eikonal_50.root"); f_objs.push("<model>/"+formFactor+"/la=3.0E-05/CH/g_dsdt"); f_pens.push(red); f_labels.push("version 50");
f_files.push("test_cni_eikonal_10.root"); f_objs.push("<model>/"+formFactor+"/la=3.0E-05/CH/g_dsdt"); f_pens.push(blue); f_labels.push("version 10");
f_files.push("test_cni_eikonal_7.root"); f_objs.push("<model>/"+formFactor+"/la=3.0E-05/CH/g_dsdt"); f_pens.push(black); f_labels.push("version 7");
f_files.push("test_cni_eikonal_5.root"); f_objs.push("<model>/"+formFactor+"/la=3.0E-05/CH/g_dsdt"); f_pens.push(green); f_labels.push("version 5");

int ref_idx = 0;

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

	for (int fi : f_files.keys)
	{
		string label = f_labels[fi];
		if (fi == ref_idx)
			label += " [ref]";

		pen p = f_pens[fi];

		string dir = replace(f_objs[fi], "<model>", models[mi]);
		draw(RootGetObject(topDir + f_files[fi], dir), p, label);
	}

	limits((0, 300), (0.02, 1000), Crop);
}

AttachLegend();

NewRow();

//yTicksDef = RightTicks(0.001, 0.0005);

for (int mi : models.keys)
{
	NewPad("$|t|\ung{GeV^2}$", "$(\d\sigma/\d t - \hbox{ref}) / \hbox{ref}$");
	//scale(Linear, Log);

	for (int fi : f_files.keys)
	{
		pen p = f_pens[fi];

		string dir = replace(f_objs[fi], "<model>", models[mi]);
		RootObject obj = RootGetObject(topDir + f_files[fi], dir);

		string dir_ref = replace(f_objs[ref_idx], "<model>", models[mi]);
		RootObject obj_ref = RootGetObject(topDir + f_files[ref_idx], dir_ref);

		DrawRel(obj, obj_ref, p);
	}

	limits((0, -0.001), (0.02, 0.001), Crop);
}

GShipout(vSkip=0mm);
