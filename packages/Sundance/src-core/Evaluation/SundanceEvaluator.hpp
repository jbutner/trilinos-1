/* @HEADER@ */
/* @HEADER@ */

#ifndef SUNDANCE_EVALUATOR_H
#define SUNDANCE_EVALUATOR_H

#include "SundanceDefs.hpp"
#include "Teuchos_RefCountPtr.hpp"
#include "SundanceEvalVector.hpp"
#include "TSFObjectWithVerbosity.hpp"
#include "SundanceSparsitySuperset.hpp"
#include "SundanceSparsitySubset.hpp"
#include "SundanceSet.hpp"
#include "SundanceMultiIndex.hpp"
#include "SundanceTabs.hpp"
#include "SundanceOut.hpp"


namespace SundanceCore 
{
  class CoordExpr;

  namespace Internal
  {
    class EvalContext;
  }
  
  using namespace Internal;

  namespace Internal 
  {
    class EvalManager;
    class SumExpr;
    class ProductExpr;
    class DiffOp;
    class UnaryMinus;
    class SpatiallyConstantExpr;
    class SymbolicFuncElement;
    class DiscreteFuncElement;
    class NonlinearUnaryOp;
    class EvaluatableExpr;
    class MultiIndex;


    /**
     * Base class for evaluator objects. Each EvaluatableExpr type will 
     * have an associated Evaluator subtype.
     */
    class Evaluator : public TSFExtended::ObjectWithVerbosity<Evaluator>
    {
    public:
      /** */
      Evaluator();

      /** */
      virtual ~Evaluator(){;}

      /** 
       * Client-level evaluation method. Computes new results on the
       * first call, makes copies on subsequent calls up to the last client, 
       * and finally returns the original result vector upon the 
       * last client's call. 
       */
      void eval(const EvalManager& mgr,
                Array<double>& constantResults,
                Array<RefCountPtr<EvalVector> >& vectorResults) const ;

      /** Reset the number of calls to zero. This should be called
       * at the beginning of every new evaluation cycle. */
      virtual void resetNumCalls() const {numCalls_=0;}

      /** */
      virtual void 
      internalEval(const EvalManager& mgr,
                   Array<double>& constantResults,
                   Array<RefCountPtr<EvalVector> >& vectorResults) const = 0 ;

      /** Add one to the number of clients. */
      void addClient() {numClients_++;}

      /** */
      void addConstantIndex(int index, int constantIndex);

      /** */
      void addVectorIndex(int index, int vectorIndex);

      

      /** */
      const Map<int, int>& constantIndexMap() const 
      {return constantIndexMap_;}

      /** */
      const Map<int, int>& vectorIndexMap() const 
      {return vectorIndexMap_;}
    protected:

      /** Return the number of clients that will require results
       * from this evaluator */
      int numClients() const {return numClients_;}

      /** */
      bool isOne(int x) const {return x==1;}

      /** */
      bool isOne(const double& x) const {return isZero(x-1.0);}

      /** */
      bool isZero(const double& x) const {return fabs(x-0.0)<1.0e-15;}

      /** */
      const Array<int>& constantIndices() const {return constantIndices_;}

      /** */
      const Array<int>& vectorIndices() const {return vectorIndices_;}


    private:
      int numClients_;

      mutable int numCalls_;

      mutable Array<RefCountPtr<EvalVector> > vectorResultCache_;

      mutable Array<double> constantResultCache_;

      Map<int, int> constantIndexMap_;

      Map<int, int> vectorIndexMap_;

      Array<int> vectorIndices_;

      Array<int> constantIndices_;
    };

    /**
     * 
     */
    template <class ExprType> class SubtypeEvaluator : public Evaluator
    {
    public:
      /** */
      SubtypeEvaluator(const ExprType* expr,
                       const EvalContext& context)
        : Evaluator(), 
          expr_(expr), 
          sparsity_(expr_->sparsitySuperset(context))
      {
        verbosity() = Evaluator::classVerbosity();
      }

      /** */
      virtual ~SubtypeEvaluator(){;}

    protected:
      
      /** */
      const RefCountPtr<SparsitySuperset>& sparsity() const {return sparsity_;}

      /** */
      const ExprType* expr() const {return expr_;}

      /** */
      const MultipleDeriv& vectorResultDeriv(int iVec) const 
      {
        return sparsity()->deriv(vectorIndices()[iVec]);
      }

      /** */
      const MultipleDeriv& constantResultDeriv(int iConst) const 
      {
        return sparsity()->deriv(constantIndices()[iConst]);
      }

    private:
      const ExprType* expr_;

      RefCountPtr<SparsitySuperset> sparsity_;
    };

    /**
     * 
     */
    template <class ExprType> class BinaryEvaluator 
      : public SubtypeEvaluator<ExprType>
    {
    public:
      /** */
      BinaryEvaluator(const ExprType* expr,
                      const EvalContext& context)
        : SubtypeEvaluator<ExprType>(expr, context),
          leftExpr_(expr->leftEvaluatable()),
          rightExpr_(expr->rightEvaluatable()),
          leftSparsity_(leftExpr_->sparsitySuperset(context)),
          rightSparsity_(rightExpr_->sparsitySuperset(context)),
          leftEval_(leftExpr_->evaluator(context)),
          rightEval_(rightExpr_->evaluator(context))
      {
        leftEval_->addClient();
        rightEval_->addClient();
      }

      /** */
      virtual ~BinaryEvaluator(){;}

      /** */
      virtual void resetNumCalls() const 
      {
        leftEval_->resetNumCalls();
        rightEval_->resetNumCalls();
        Evaluator::resetNumCalls();
      }

    protected:
      
      /** */
      const RefCountPtr<SparsitySuperset>& leftSparsity() const 
      {return leftSparsity_;}
      
      /** */
      const RefCountPtr<SparsitySuperset>& rightSparsity() const 
      {return rightSparsity_;}

      /** */
      const EvaluatableExpr* leftExpr() const {return leftExpr_;}

      /** */
      const EvaluatableExpr* rightExpr() const {return rightExpr_;}

      /** */
      const RefCountPtr<Evaluator>& leftEval() const {return leftEval_;}

      /** */
      const RefCountPtr<Evaluator>& rightEval() const {return rightEval_;}

      /** */
      void evalChildren(const EvalManager& mgr,
                        Array<double>& leftConstResults,
                        Array<RefCountPtr<EvalVector> >& leftVecResults,
                        Array<double>& rightConstResults,
                        Array<RefCountPtr<EvalVector> >& rightVecResults) const 
      {
        Tabs tabs;
        SUNDANCE_OUT(verbosity() > VerbLow, 
                     tabs << "Evaluating left and right children: "
                     << endl << tabs << "left=" << leftExpr_->toString()
                     << endl << tabs << "right=" << rightExpr_->toString());
        SUNDANCE_OUT(verbosity() > VerbLow, 
                     tabs << "Evaluating left=" << leftExpr_->toString());
        leftEval()->eval(mgr, leftConstResults, leftVecResults);
        
        SUNDANCE_OUT(verbosity() > VerbLow, 
                     tabs << "Evaluating right=" << rightExpr_->toString());
        rightEval()->eval(mgr, rightConstResults, rightVecResults);
      }

    private:
      const EvaluatableExpr* leftExpr_;

      const EvaluatableExpr* rightExpr_;

      RefCountPtr<SparsitySuperset> leftSparsity_;

      RefCountPtr<SparsitySuperset> rightSparsity_;

      RefCountPtr<Evaluator> leftEval_;

      RefCountPtr<Evaluator> rightEval_;
    };

    /**
     * 
     */
    template <class ExprType> class UnaryEvaluator 
      : public SubtypeEvaluator<ExprType>
    {
    public:
      /** */
      UnaryEvaluator(const ExprType* expr,
                     const EvalContext& context)
        : SubtypeEvaluator<ExprType>(expr, context),
          argExpr_(expr->evaluatableArg()),
          argSparsity_(argExpr_->sparsitySubset(context, 
                                                expr->argMultiIndices(sparsity()->allMultiIndices()))),
          argEval_(argExpr_->evaluator(context))
      {
        argEval_->addClient();
      }

      /** */
      virtual ~UnaryEvaluator(){;}

      /** */
      virtual void resetNumCalls() const 
      {
        argEval_->resetNumCalls();
        Evaluator::resetNumCalls();
      }

    protected:

      /** */
      const RefCountPtr<SparsitySubset>& argSparsity() const 
      {return argSparsity_;}
      
      /** */
      const EvaluatableExpr* argExpr() const {return argExpr_;}

      /** */
      const RefCountPtr<Evaluator>& argEval() const {return argEval_;}
      

      /** */
      void evalOperand(const EvalManager& mgr,
                       Array<double>& argConstantResults,
                       Array<RefCountPtr<EvalVector> >& argVectorResults) const 
      {
        Tabs tabs;
        SUNDANCE_OUT(verbosity() > VerbLow, 
                     tabs << "Evaluating operand: ");
        argEval()->eval(mgr, argConstantResults, argVectorResults);
      }
    private:
      const EvaluatableExpr* argExpr_;

      RefCountPtr<SparsitySubset> argSparsity_;

      RefCountPtr<Evaluator> argEval_;
    };



    

  }
}


#endif
