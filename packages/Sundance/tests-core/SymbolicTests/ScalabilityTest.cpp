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
#include "Teuchos_MPISession.hpp"
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
using namespace SundanceCore::Internal;
using namespace Teuchos;
using namespace TSFExtended;

static Time& totalTimer() 
{
  static RefCountPtr<Time> rtn 
    = TimeMonitor::getNewTimer("total"); 
  return *rtn;
}




int main(int argc, void** argv)
{
  
  try
		{
      MPISession::init(&argc, &argv);
      Tabs tabs;
      TimeMonitor timer(totalTimer());

      //       verbosity<SymbolicTransformation>() = VerbSilent;
      //       verbosity<EvaluationTester>() = VerbExtreme;
      //       verbosity<Evaluator>() = VerbExtreme;
      //       verbosity<EvalVector>() = VerbExtreme;
      //       verbosity<EvaluatableExpr>() = VerbExtreme;
      //       verbosity<AbstractEvalMediator>() = VerbExtreme;
      Expr::showAllParens() = true;

      EvalVector::shadowOps() = false;

      Expr dx = new Derivative(0);
      Expr dy = new Derivative(1);
      Expr dz = new Derivative(2);

      Expr x = new CoordExpr(0);
      Expr y = new CoordExpr(1);
      Expr z = new CoordExpr(2);

      ADCoord X(0);
      ADCoord Y(1);
      ADCoord Z(2);

      ADDerivative Dx(0);
      ADDerivative Dy(1);
      ADDerivative Dz(2);

      Time stopwatch("test");

      for (int n=2; n<30; n++)
        {
          double t0 = stopwatch.wallTime();
          stopwatch.start();
          int numFields = n;

          Array<ADField> U(numFields);
          Array<Expr> u(numFields);

          Expr big = 0.0;
          ADReal value;
          for (int i=0; i<numFields; i++)
            {
              U[i] = ADField(ADBasis(i+1), ::sqrt(i+1.0));
              u[i] = new TestUnknownFunction(U[i], 
                                             "u" + Teuchos::toString(i) + "_");
              big = big + u[i]*u[i]*x;
              value = value + U[i]*U[i]*X;
              if (i > 0) 
                {
                  big = big + (dx*u[i-1])*(dy*u[i]) 
                    + (dy*u[i-1])*(dz*u[i])
                    + (dx*u[i-1])*(dz*u[i]);
                  value = value + (Dx*U[i-1])*(Dy*U[i])
                    + (Dy*U[i-1])*(Dz*U[i])
                    + (Dx*U[i-1])*(Dz*U[i]);
                }
            }

          EvaluationTester tester(big);
          Array<double> df;
          Array<Array<double> > df2;
          tester.evaluate(df, df2);
          stopwatch.stop();
          double t1 = stopwatch.wallTime();
          int nNodes = tester.numNodes();
          int nnz = tester.numNonzeros();
          cerr << n << "   " << nNodes << "   " 
               << nnz << "    " << t1-t0 << endl;

        }
    }
	catch(exception& e)
		{
			Out::println(e.what());
		}
  TimeMonitor::summarize();

  MPISession::finalize();
}
