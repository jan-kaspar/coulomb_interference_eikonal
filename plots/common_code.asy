string models[], m_labels[];
models.push("exp3_con"); m_labels.push("central");
models.push("exp3_per"); m_labels.push("peripheral");

string lambdas[], l_labels[];
real l_values[];
lambdas.push("1.0E-03"); l_labels.push("$\la = 1\cdot 10^{-3}$"); l_values.push(1.0e-03);
lambdas.push("1.0E-04"); l_labels.push("$\la = 1\cdot 10^{-4}$"); l_values.push(1.0e-04);
//lambdas.push("3.0E-05"); l_labels.push("$\la = 3\cdot 10^{-5}$"); l_values.push(3.0e-05);
//lambdas.push("1.0E-05"); l_labels.push("$\la = 1\cdot 10^{-6}$"); l_values.push(1.0e-05);

string lambda_ref = "1.0E-04";

string formFactors[], ff_labels[];
formFactors.push("none"); ff_labels.push("no form-factor");
formFactors.push("Puckett"); ff_labels.push("Puckett");

string formFactor = "Puckett";

//----------------------------------------------------------------------------------------------------

void DrawRel(RootObject o, RootObject o_ref, pen p, string label="")
{
	guide g;

	int n = o.iExec("GetN");
	for (int i = 0; i < n; ++i)
	{
		real ax[] = {0.};
		real ay[] = {0.};

		o.vExec("GetPoint", i, ax, ay);
		real x = ax[0];
		real y = ay[0];

		real y_ref = o_ref.rExec("Eval", x);

		real y_rel = (y - y_ref) / y_ref;

		g = g--(x, y_rel);
	}

	draw(g, p, label);
}

