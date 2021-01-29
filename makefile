HepMC_DIR = /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/hepmc/2.06.07-bcolbf2

GSL_DIR = /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/gsl/2.6-ghbfee

Elegent_DIR = /afs/cern.ch/work/j/jkaspar/work/software/Elegent/production/CMSSW_11_0_0/install

INCS = `root-config --cflags` -I$(GSL_DIR)/include -I${Elegent_DIR} -I$(HepMC_DIR)/include
LIBS = `root-config --libs` -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm -L${Elegent_DIR}/lib -lElegent -L$(HepMC_DIR)/lib -lHepMC

all: test_cni_terms test_hadronic test_cni_elegent test_cni_eikonal test_cni_petrov

test_cni_terms: test_cni_terms.cc
	g++ --std=c++11 -O3 -Wall -Wextra $(INCS) $(LIBS) -Wl,-rpath=${Elegent_DIR}/lib \
		test_cni_terms.cc -o test_cni_terms

test_hadronic: test_hadronic.cc HadronicFitModel.h HadronicModels.h
	g++ --std=c++11 -O3 -Wall -Wextra $(INCS) $(LIBS) -Wl,-rpath=${Elegent_DIR}/lib \
		test_hadronic.cc -o test_hadronic

test_cni_elegent: test_cni_elegent.cc HadronicFitModel.h HadronicModels.h
	g++ --std=c++11 -O3 -Wall -Wextra $(INCS) $(LIBS) -Wl,-rpath=${Elegent_DIR}/lib \
		test_cni_elegent.cc -o test_cni_elegent

test_cni_eikonal: test_cni_eikonal.cc HadronicFitModel.h HadronicModels.h
	g++ --std=c++11 -O3 -Wall -Wextra $(INCS) $(LIBS) -Wl,-rpath=${Elegent_DIR}/lib \
		test_cni_eikonal.cc -o test_cni_eikonal

test_cni_petrov: test_cni_petrov.cc HadronicFitModel.h HadronicModels.h
	g++ --std=c++11 -O3 -Wall -Wextra $(INCS) $(LIBS) -Wl,-rpath=${Elegent_DIR}/lib \
		test_cni_petrov.cc -o test_cni_petrov
