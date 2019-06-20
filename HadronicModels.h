#include "HadronicFitModel.h"


HadronicFitModel *m_exp3_con = NULL;
HadronicFitModel *m_exp3_per = NULL;

void InitHadronicModels()
{
	m_exp3_con = new HadronicFitModel();
	m_exp3_con->Init();
	m_exp3_con->shortLabel.name = "exp3_con";

	m_exp3_con->t1 = 0.2;
	m_exp3_con->t2 = 0.5;
	m_exp3_con->phaseMode = HadronicFitModel::pmConstant;

	m_exp3_con->a=6.779E+08;
	m_exp3_con->b1=1.023E+01;
	m_exp3_con->b2=4.407E+00;
	m_exp3_con->b3=1.018E+01;

	m_exp3_con->p0=1.452E+00;

	// -----

	m_exp3_per = new HadronicFitModel();
	m_exp3_per->Init();
	m_exp3_per->shortLabel.name = "exp3_per";

	m_exp3_per->t1 = 0.2;
	m_exp3_per->t2 = 0.5;
	m_exp3_per->phaseMode = HadronicFitModel::pmPeripheral;

	m_exp3_per->a=6.780E+08;
	m_exp3_per->b1=9.780E+00;
	m_exp3_per->b2=-1.646E+00;
	m_exp3_per->b3=-6.456E+00;

	m_exp3_per->p0=1.454E+00;
	m_exp3_per->p_A=4.300E+00;
	m_exp3_per->p_ka=2.311E+00;
	m_exp3_per->p_tm=-2.831E-01;
}
