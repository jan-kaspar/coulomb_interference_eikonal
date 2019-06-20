import root;
import pad_layout;
include "common_code.asy";

string topDir = "../";

string f_input = topDir + "test_hadronic.root";
string f_eikonal = topDir + "test_cni_eikonal.root";

string lambda = "1.0E-02";

string quantities[], q_labels[], q_units[];
real q_abs_mins[], q_abs_maxs[], q_rel_maxs[];
quantities.push("dsdt"); q_labels.push("\d\sigma/\d t"); q_units.push("\ung{mb/GeV^2}"); q_abs_mins.push(0.); q_abs_maxs.push(600.); q_rel_maxs.push(0.001);
quantities.push("phase"); q_labels.push("\arg F^{\rm H}"); q_units.push(""); q_abs_mins.push(-3.5); q_abs_maxs.push(3.5); q_rel_maxs.push(0.001);

ySizeDef = 5cm;
xTicksDef = LeftTicks(0.05, 0.01);

//----------------------------------------------------------------------------------------------------

for (int qi : quantities.keys)
{
	string quantity = quantities[qi];

	for (int mi : models.keys)
	{
		NewPad(false);
		label(m_labels[mi]);
	}

	NewRow();

	for (int mi : models.keys)
	{
		NewPad("$|t|\ung{GeV^2}$", "$" + q_labels[qi] + q_units[qi] + "$");
		//scale(Linear, Log);

		RootObject g_input = RootGetObject(f_input, models[mi] + "/g_" + quantity);
		RootObject g_eikonal = RootGetObject(f_eikonal, models[mi] + "/" + formFactor + "/la=" + lambda + "/H/g_" + quantity);

		draw(g_input, red, "input");

		draw(g_eikonal, blue + dashed, "eikonal calculation");

		limits((0, q_abs_mins[qi]), (0.2, q_abs_maxs[qi]), Crop);
	}

	AttachLegend();

	NewRow();

	for (int mi : models.keys)
	{
		NewPad("$|t|\ung{GeV^2}$", "$(" + q_labels[qi] + " - \hbox{ref}) / \hbox{ref}$");

		RootObject g_input = RootGetObject(f_input, models[mi] + "/g_" + quantity);
		RootObject g_eikonal = RootGetObject(f_eikonal, models[mi] + "/" + formFactor + "/la=" + lambda + "/H/g_" + quantity);

		DrawRel(g_input, g_input, red);

		DrawRel(g_eikonal, g_input, blue + dashed);

		limits((0, -q_rel_maxs[qi]), (0.2, +q_rel_maxs[qi]), Crop);
	}

	AttachLegend();

	GShipout("hadr_t_cmp_input_eikonal_" + quantity, vSkip=0mm);
}
