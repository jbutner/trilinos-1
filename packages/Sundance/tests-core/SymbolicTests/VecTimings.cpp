#include "SundanceExpr.hpp"
#include "SundanceStdMathOps.hpp"
#include "SundanceDerivative.hpp"
#include "SundanceUnknownFunctionStub.hpp"
#include "SundanceTestFunctionStub.hpp"
#include "SundanceDiscreteFunctionStub.hpp"
#include "SundanceCoordExpr.hpp"
#include "SundanceZeroExpr.hpp"
#include "SundanceSymbolicTransformation.hpp"
#include "SundanceDeriv.hpp"
#include "SundanceParameter.hpp"
#include "SundanceOut.hpp"
#include "Teuchos_Time.hpp"
#include "Teuchos_GlobalMPISession.hpp"
#include "Teuchos_TimeMonitor.hpp"
#include "SundanceDerivSet.hpp"
#include "SundanceRegionQuadCombo.hpp"
#include "SundanceEvalManager.hpp"
#include "SundanceEvalVector.hpp"
#include "SundanceSymbPreprocessor.hpp"
#include "SundanceStringEvalMediator.hpp"
#include "SundanceEvaluationTester.hpp"

using namespace SundanceUtils;
using namespace SundanceTesting;
using namespace SundanceCore;
using namespace SundanceCore;
using namespace Teuchos;


typedef RefCountPtr<EvalVector> Vec;

static Time& totalTimer() 
{
  static RefCountPtr<Time> rtn 
    = TimeMonitor::getNewTimer("total"); 
  return *rtn;
}


double rawOps(const Array<Vec>& vecs, int nTrials, double alpha, double beta, 
              double gamma)
{
  double* a = vecs[0]->start();
  const double* b = vecs[1]->start();
  const double* c = vecs[2]->start();
  const double* d = vecs[3]->start();
  int n = vecs[0]->length();

  Time t("loop");
  double t0 = t.totalElapsedTime();
  t.start();

  for (int m=0; m<nTrials; m++)
    {
      for (int i=0; i<n; i++)
        {
          a[i] = alpha*b[i] + beta*c[i]*d[i];
          a[i] = b[i]*a[i] + gamma*c[i]*d[i];
          a[i] = b[i]*a[i];
          a[i] = alpha*a[i] + beta*c[i];
          a[i] = alpha*b[i] + beta*c[i]*d[i];
          a[i] = b[i]*a[i] + gamma*c[i]*d[i];
          a[i] = b[i]*a[i];
          a[i] = alpha*a[i] + beta*c[i];
          a[i] = alpha*b[i] + beta*c[i]*d[i];
          a[i] = b[i]*a[i] + gamma*c[i]*d[i];
          a[i] = b[i]*a[i];
          a[i] = alpha*a[i] + beta*c[i];
          a[i] = alpha*b[i] + beta*c[i]*d[i];
          a[i] = b[i]*a[i] + gamma*c[i]*d[i];
          a[i] = b[i]*a[i];
          a[i] = alpha*a[i] + beta*c[i];
        }
    }
  t.stop();
  return t.totalElapsedTime() - t0;
}

double blockOps(const Array<Vec>& vecs, int nTrials, 
		  double alpha, double beta, 
		  double gamma)
{
  int n = vecs[0]->length();
  Array<double> data(4*n);
  for (int i=0; i<4; i++)
    {
      for (int j=0; j<n; j++)
	{
	  data[j*4+i] = vecs[i]->start()[j];
	}
    }
  
  Time t("block");
  double t0 = t.totalElapsedTime();
  t.start();

  for (int m=0; m<nTrials; m++)
    {
      for (int i=0; i<n; i++)
        {
	  double* p = &(data[4*i]);
	  double& a = p[0];
	  const double& b = p[1];
	  const double& c = p[2];
	  const double& d = p[3];
          a = alpha*b + beta*c*d;
          a = b*a + gamma*c*d;
          a = b*a;
          a = alpha*a + beta*c;
          a = alpha*b + beta*c*d;
          a = b*a + gamma*c*d;
          a = b*a;
          a = alpha*a + beta*c;
          a = alpha*b + beta*c*d;
          a = b*a + gamma*c*d;
          a = b*a;
          a = alpha*a + beta*c;
          a = alpha*b + beta*c*d;
          a = b*a + gamma*c*d;
          a = b*a;
          a = alpha*a + beta*c;
        }
    }
  t.stop();
  return t.totalElapsedTime() - t0;
}

double funcOps(const Array<Vec>& vecs, int nTrials, double alpha, double beta, 
               double gamma)
{
  Array<int> abcd = tuple(0,1,2,3);
  int n = vecs[0]->length();

  Time t("loop");
  double t0 = t.totalElapsedTime();
  t.start();

  for (int m=0; m<nTrials; m++)
    {
      Vec A = vecs[abcd[0]];
      {
        const Vec& B = vecs[abcd[1]];
        A->setTo_SV(alpha, B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->add_SVV(beta, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->multiply_V_add_SVV(B.get(), gamma, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        A->multiply_V(B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        A->multiply_S_add_SV(alpha, beta, C.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        A->setTo_SV(alpha, B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->add_SVV(beta, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->multiply_V_add_SVV(B.get(), gamma, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        A->multiply_V(B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        A->multiply_S_add_SV(alpha, beta, C.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        A->setTo_SV(alpha, B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->add_SVV(beta, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->multiply_V_add_SVV(B.get(), gamma, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        A->multiply_V(B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        A->multiply_S_add_SV(alpha, beta, C.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        A->setTo_SV(alpha, B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->add_SVV(beta, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        const Vec& C = vecs[abcd[2]];
        const Vec& D = vecs[abcd[3]];
        A->multiply_V_add_SVV(B.get(), gamma, C.get(), D.get());
      }
      {
        const Vec& B = vecs[abcd[1]];
        A->multiply_V(B.get());
      }
      {
        const Vec& C = vecs[abcd[2]];
        A->multiply_S_add_SV(alpha, beta, C.get());
      }
    }
  t.stop();
  return t.totalElapsedTime() - t0;
}

int main(int argc, char** argv)
{
  
  try
		{
      GlobalMPISession session(&argc, &argv);
      Tabs tabs;
      TimeMonitor timer(totalTimer());

      

      EvalVector::shadowOps() = false;
      Array<string> failures;
      
      int nSamples = 5000;

      for (int n=1; n<32768; n*=2)
        {
          TempStack s(n);
          int nTests = 2;

          Vec A = s.popVector();
          Vec B = s.popVector();
          Vec C = s.popVector();
          Vec D = s.popVector();
          Array<Vec> vecs = tuple(A, B, C, D);

          double alpha = 2.0;
          double beta = 4.25;
          double gamma = -1.2;



          Array<double> tRaw(nSamples);
          Array<double> tFunc(nSamples);
          Array<double> tBlock(nSamples);
          Array<double> r(nSamples);

          double tRawMean = 0.0;
          double tFuncMean = 0.0;
          double tBlockMean = 0.0;

          for (int i=0; i<nSamples; i++)
            {
              tRaw[i] = rawOps(vecs, nTests, alpha, beta, gamma);
              tFunc[i] = funcOps(vecs, nTests, alpha, beta, gamma);
              tBlock[i] = blockOps(vecs, nTests, alpha, beta, gamma);
              tRawMean += tRaw[i];
              tFuncMean += tFunc[i];
              tBlockMean += tBlock[i];
            }

          tRawMean /= ((double) nSamples);
          tFuncMean /= ((double) nSamples);
          tBlockMean /= ((double) nSamples);

          double varRaw = 0.0;
          double varFunc = 0.0;
          double varBlock = 0.0;
          for (int i=0; i<nSamples; i++)
            {
              varRaw += pow(tRaw[i] - tRawMean, 2.0);
              varFunc += pow(tFunc[i] - tFuncMean, 2.0);
              varBlock += pow(tBlock[i] - tFuncMean, 2.0);
            }

          double rms1 = sqrt((varRaw + varFunc)/((double) nSamples))/tRawMean;
          double rms2 = sqrt((varRaw + varBlock)/((double) nSamples))/tRawMean;
          double ratio1 = tFuncMean/tRawMean;
          double ratio2 = tBlockMean/tRawMean;

          cerr << n << "\t  " << ratio1 << "\t  " << rms1 << "\t  "
	       << ratio2 << "\t  " << rms2 << endl;
        }
      TimeMonitor::summarize();
    }
	catch(exception& e)
		{
			Out::println(e.what());
		}


  
}