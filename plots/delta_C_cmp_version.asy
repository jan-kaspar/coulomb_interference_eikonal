import root;
import pad_layout;
include "common_code.asy";

string topDir = "../";

string model = "exp3_con";	// doesn't matter

string lambda = "1.0E-04";

string f_files[], f_objs[], f_labels[];
pen f_pens[];

f_files.push("test_q_max/test_cni_eikonal_OLD.root"); f_objs.push(model + "/<formFactor>/la=" + lambda + "/de_C_re"); f_pens.push(red); f_labels.push("OLD");
f_files.push("test_q_max/test_cni_eikonal_OLD2.root"); f_objs.push(model + "/<formFactor>/la=" + lambda + "/de_C_re"); f_pens.push(blue); f_labels.push("OLD2");
//f_files.push("test_q_max/test_cni_eikonal_NEW_1E-5.root"); f_objs.push(model + "/<formFactor>/la=" + lambda + "/de_C_re"); f_pens.push(blue); f_labels.push("NEW, 1E-5");
//f_files.push("test_q_max/test_cni_eikonal_NEW_1E-6.root"); f_objs.push(model + "/<formFactor>/la=" + lambda + "/de_C_re"); f_pens.push(heavygreen); f_labels.push("NEW, 1E-6");
//f_files.push("test_q_max/test_cni_eikonal_NEW_1E-7.root"); f_objs.push(model + "/<formFactor>/la=" + lambda + "/de_C_re"); f_pens.push(magenta); f_labels.push("NEW, 1E-7");

int ref_idx = 0;

ySizeDef = 5cm;

//xTicksDef = LeftTicks(5., 1.);

//----------------------------------------------------------------------------------------------------

for (int ffi : formFactors.keys)
{
	NewPad(false);
	label(ff_labels[ffi]);
}

NewRow();

for (int ffi : formFactors.keys)
{
	NewPad("$b\ung{GeV^{-2}}$", "$\de^{\rm C}(b)$");

	for (int fli : f_files.keys)
	{
		string label = f_labels[fli];
		if (fli == ref_idx)
			label += " [ref]";

		string objName = replace(f_objs[fli], "<formFactor>", formFactors[ffi]);
		RootObject g = RootGetObject(topDir + f_files[fli], objName);
		draw(g, f_pens[fli], label);
	}

	limits((0, -0.2), (20., 0.), Crop);
}

AttachLegend();

NewRow();

for (int ffi : formFactors.keys)
{
	NewPad("$b\ung{GeV^{-2}}$", "$(\de^{\rm C} - \hbox{ref}) / \hbox{ref}$");

	for (int fli : f_files.keys)
	{
		string objName = replace(f_objs[fli], "<formFactor>", formFactors[ffi]);
		RootObject g = RootGetObject(topDir + f_files[fli], objName);

		string objName_ref = replace(f_objs[ref_idx], "<formFactor>", formFactors[ffi]);
		RootObject g_ref = RootGetObject(topDir + f_files[ref_idx], objName_ref);

		DrawRel(g, g_ref, f_pens[fli]);
	}

	xlimits(0, 0.001, Crop);
	//limits((0, -0.0001), (0.001, 0.0001), Crop);
	//limits((0, -0.001), (20., 0.001), Crop);
}

GShipout(vSkip=0mm);
