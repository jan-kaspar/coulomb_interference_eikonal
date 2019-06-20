#include "TGraph.h"

class Interpolator
{
	public:
		typedef double (*DoubleFcn)(double);
		typedef TComplex (*TComplexFcn)(double);

		unsigned int N;
		double x_min, st;
		bool logS;

		std::vector<double> re, im;

		Interpolator(unsigned _N, double _x_min, double _x_max, bool _log);

		void Sample(DoubleFcn f);
		void Sample(TComplexFcn f);
		
		double EvalRe(double x);
		TComplex Eval(double x);

		TGraph* GraphRe(const char *name = "");
		TGraph* GraphIm(const char *name = "");
		TGraph* GraphMod(const char *name = "");
		TGraph* GraphArg(const char *name = "");
};

//----------------------------------------------------------------------------------------------------

Interpolator::Interpolator(unsigned _N, double _x_min, double _x_max, bool _log) :
			N(_N), x_min(_x_min), logS(_log)
{
	st = (logS) ? log(_x_max / _x_min) / (N-1) : (_x_max - _x_min) / (N-1);
}

//----------------------------------------------------------------------------------------------------

void Interpolator::Sample(Interpolator::DoubleFcn fcn)
{
	double x = x_min;
	double f = exp(st);
	re.clear();
	im.clear();

	for (unsigned int i = 0; i < N; i++, (logS) ? x *= f : x += st) {
		re.push_back(fcn(x));
		im.push_back(0.);
	}
}

//----------------------------------------------------------------------------------------------------

void Interpolator::Sample(Interpolator::TComplexFcn fcn)
{
	double x = x_min;
	double f = exp(st);
	re.clear();
	im.clear();

	for (unsigned int i = 0; i < N; i++, (logS) ? x *= f : x += st) {
		TComplex z = fcn(x);
		re.push_back(z.Re());
		im.push_back(z.Im());
	}
}

//----------------------------------------------------------------------------------------------------

double Interpolator::EvalRe(double x)
{
  double v = (logS) ? log(x/x_min) : x - x_min;
  signed int idx = (int)(v / st);
  if (idx < 0 || (unsigned int)(idx + 1) > N - 1)
    return 0.;

  double f = v/st - idx;

  return (re[idx+1] - re[idx])*f + re[idx];
}


//----------------------------------------------------------------------------------------------------

TComplex Interpolator::Eval(double x)
{
  double v = (logS) ? log(x/x_min) : x - x_min;
  signed int idx = (int)(v / st);
  if (idx < 0 || (unsigned int)(idx + 1) > N - 1)
    return TComplex(0, 0);

  double f = v/st - idx;

  return TComplex(
    (re[idx+1] - re[idx])*f + re[idx],
    (im[idx+1] - im[idx])*f + im[idx]
  );
}

//----------------------------------------------------------------------------------------------------

TGraph* Interpolator::GraphRe(const char *name)
{
	TGraph *g = new TGraph();
	double x = x_min;
	double f = exp(st);
	for (unsigned int i = 0; i < N; i++, (logS) ? x *= f : x += st) {
		g->SetPoint(i, x, re[i]);
	}
	if (strlen(name) > 0)
		g->SetName(name);
	return g;
}

//----------------------------------------------------------------------------------------------------

TGraph* Interpolator::GraphIm(const char *name)
{
	TGraph *g = new TGraph();
	double x = x_min;
	double f = exp(st);
	for (unsigned int i = 0; i < N; i++, (logS) ? x *= f : x += st) {
		g->SetPoint(i, x, im[i]);
	}
	if (strlen(name) > 0)
		g->SetName(name);
	return g;
}

//----------------------------------------------------------------------------------------------------

TGraph* Interpolator::GraphMod(const char *name)
{
	TGraph *g = new TGraph();
	double x = x_min;
	double f = exp(st);
	for (unsigned int i = 0; i < N; i++, (logS) ? x *= f : x += st) {
		g->SetPoint(i, x, TComplex(re[i], im[i]).Rho());
	}
	if (strlen(name) > 0)
		g->SetName(name);
	return g;
}

//----------------------------------------------------------------------------------------------------

TGraph* Interpolator::GraphArg(const char *name)
{
	TGraph *g = new TGraph();
	double x = x_min;
	double f = exp(st);
	for (unsigned int i = 0; i < N; i++, (logS) ? x *= f : x += st) {
		g->SetPoint(i, x, TComplex(re[i], im[i]).Theta());
	}
	if (strlen(name) > 0)
		g->SetName(name);
	return g;
}
