/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//                                                                                 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA                                                                                
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#include "SundanceFunctionalPolynomial.hpp"
#include "SundanceTabs.hpp"
#include "SundanceOut.hpp"
#include "SundanceExpr.hpp"
#include "SundanceEvaluatorFactory.hpp"
#include "SundanceEvaluator.hpp"
#include "SundanceSymbolicFuncElement.hpp"
#include "SundanceDerivOfSymbFunc.hpp"
#include "SundanceUnaryExpr.hpp"


using namespace SundanceCore;
using namespace SundanceUtils;

using namespace SundanceCore;
using namespace Teuchos;



FunctionalPolynomial::FunctionalPolynomial(const RefCountPtr<ScalarExpr>& expr)
  : funcs_(),
    funcMultiIndices_(),
    coeffs_(),
    keys_()
{
  TEST_FOR_EXCEPTION(!isConvertibleToPoly(expr.get()), InternalError, 
                     "FunctionalPolynomial ctor called with an argument that "
                     "cannot be converted to a polynomial");
  int funcID;
  MultiIndex mi;
  Deriv deriv;

  const SymbolicFuncElement* s 
    = dynamic_cast<const SymbolicFuncElement*>(expr.get());
  if (s != 0)
    {
      mi = MultiIndex();
      deriv = funcDeriv(s);
    }
  
  const DerivOfSymbFunc* d
    = dynamic_cast<const DerivOfSymbFunc*>(expr.get());
  if (d != 0)
    {
      deriv = d->representMeAsFunctionalDeriv();
    }

  MultipleDeriv mu;
  mu.put(deriv);


  if (d != 0 || s != 0)
    {
      funcs_.put(funcID, expr);
      Set<MultiIndex> miSet;
      miSet.put(mi);
      funcMultiIndices_.put(funcID, miSet);

      Expr coeff = 1.0;
      RefCountPtr<ScalarExpr> cPtr = rcp_dynamic_cast<ScalarExpr>(coeff.ptr());

      coeffs_.resize(2);
      keys_.resize(2);
      coeffs_[1].put(mu, cPtr);
      keys_[1].put(mu);
    }
  else
    {
      TEST_FOR_EXCEPTION(true, InternalError, 
                         "impossible case in FunctionalPolynomial ctor");
    }
}


FunctionalPolynomial::FunctionalPolynomial(const Map<int, RefCountPtr<ScalarExpr> >& funcs,
                                           const Map<int, Set<MultiIndex> >& funcMultiIndices,
                                           const Array<Map<MultipleDeriv, RefCountPtr<ScalarExpr> > > & coeffs)
  : funcs_(funcs),
    funcMultiIndices_(funcMultiIndices),
    coeffs_(coeffs),
    keys_(coeffs.size())
{
  typedef Map<MultipleDeriv, RefCountPtr<ScalarExpr> > termMap;

  for (unsigned int i=0; i < coeffs_.size(); i++)
    {
      for (termMap::const_iterator 
             j = coeffs_[i].begin(); j != coeffs_[i].end(); j++)
        {
          const MultipleDeriv& key = j->first;
          keys_[i].put(key);
        }
    }
}


Set<MultipleDeriv> 
FunctionalPolynomial::internalFindW(int order, const EvalContext& context) const
{
  TEST_FOR_EXCEPTION(true, InternalError, 
                     "FunctionalPolynomial not implemented");
  return Set<MultipleDeriv> ();
}


RefCountPtr<FunctionalPolynomial> FunctionalPolynomial::
addPoly(const FunctionalPolynomial* other, int sign) const 
{
  typedef Map<MultipleDeriv, RefCountPtr<ScalarExpr> > termMap;
  Map<int, RefCountPtr<ScalarExpr> > newFuncs = funcs_;
  Map<int, Set<MultiIndex> > newFuncMultiIndices = funcMultiIndices_;
  Array<Map<MultipleDeriv, RefCountPtr<ScalarExpr> > > newCoeffs = coeffs_;

  if (other->coeffs_.size() > coeffs_.size()) 
    newCoeffs.resize(other->coeffs_.size());


  for (unsigned int i=0; i < other->coeffs_.size(); i++)
    {
      
      for (termMap::const_iterator 
             j = other->coeffs_[i].begin(); j != other->coeffs_[i].end(); j++)
        {
          const MultipleDeriv& key = j->first;
          Expr right = Expr::handle(j->second);
          Expr sum;
      
          if (newCoeffs[i].containsKey(key))
            {
              Expr left = Expr::handle(newCoeffs[i].get(key));

              if (sign > 0) sum = left + right;
              else sum = left - right;
            }
          else
            {
              if (sign > 0) sum = right;
              else sum = -right;
            }
      
          const ScalarExpr* se = dynamic_cast<const ScalarExpr*>(sum.ptr().get());
          TEST_FOR_EXCEPTION(se==0, InternalError,
                             "Sum could not be cast to scalar expression");
          newCoeffs[i].put(key, rcp_dynamic_cast<ScalarExpr>(sum.ptr()));
        }
    }
  
  for (Map<int, RefCountPtr<ScalarExpr> >::const_iterator 
         i = other->funcs_.begin(); i != other->funcs_.end(); i++)
    {
      newFuncs.put(i->first, i->second);
    }

  for (Map<int, Set<MultiIndex> >::const_iterator 
         i = other->funcMultiIndices_.begin(); 
       i != other->funcMultiIndices_.end(); i++)
    {
      newFuncMultiIndices.put(i->first, i->second);
    }

  

  return rcp(new FunctionalPolynomial(newFuncs, newFuncMultiIndices, newCoeffs));
}

RefCountPtr<FunctionalPolynomial> FunctionalPolynomial::
multiplyPoly(const FunctionalPolynomial* other) const 
{
  typedef Map<MultipleDeriv, RefCountPtr<ScalarExpr> > termMap;
  Map<int, RefCountPtr<ScalarExpr> > newFuncs;
  Map<int, Set<MultiIndex> > newFuncMultiIndices;
  Array<Map<MultipleDeriv, RefCountPtr<ScalarExpr> > > newCoeffs;

  newCoeffs.resize(coeffs_.size() + other->coeffs_.size() - 1);

  for (unsigned int i=0; i < coeffs_.size(); i++)
    {
      for (unsigned int j = 0; j<other->coeffs_.size(); j++)
        {
          for (termMap::const_iterator 
                 me = coeffs_[i].begin(); me != coeffs_[i].end(); me++)
            {
              const MultipleDeriv& myKey = me->first;
              Expr myExpr = Expr::handle(me->second);
              for (termMap::const_iterator 
                     you = other->coeffs_[j].begin(); 
                   you != other->coeffs_[j].end(); you++)
                {
                  const MultipleDeriv& yourKey = you->first;
                  Expr yourExpr = Expr::handle(you->second);
                  MultipleDeriv newKey = myKey.product(yourKey);
                  int order = i+j;
                  Expr newTerm = myExpr*yourExpr;
                  if (newCoeffs[order].containsKey(newKey))
                    {
                      Expr old = Expr::handle(newCoeffs[i].get(newKey));
                      newTerm = newTerm + old;
                    }
                  const ScalarExpr* se 
                    = dynamic_cast<const ScalarExpr*>(newTerm.ptr().get());
                  TEST_FOR_EXCEPTION(se==0, InternalError,
                                     "New coeff could not be cast to scalar expression");
                  newCoeffs[order].put(newKey, 
                                       rcp_dynamic_cast<ScalarExpr>(newTerm.ptr()));
                }
            }
        }
    }
  
  for (Map<int, RefCountPtr<ScalarExpr> >::const_iterator 
         i = funcs_.begin(); i != funcs_.end(); i++)
    {
      newFuncs.put(i->first, i->second);
    }
  for (Map<int, RefCountPtr<ScalarExpr> >::const_iterator 
         i = other->funcs_.begin(); i != other->funcs_.end(); i++)
    {
      newFuncs.put(i->first, i->second);
    }

  for (Map<int, Set<MultiIndex> >::const_iterator 
         i = funcMultiIndices_.begin(); 
       i != funcMultiIndices_.end(); i++)
    {
      newFuncMultiIndices.put(i->first, i->second);
    }
  for (Map<int, Set<MultiIndex> >::const_iterator 
         i = other->funcMultiIndices_.begin(); 
       i != other->funcMultiIndices_.end(); i++)
    {
      Set<MultiIndex> miSet = i->second;
      if (newFuncMultiIndices.containsKey(i->first))
        {
          miSet.merge(newFuncMultiIndices.get(i->first));
        }
      newFuncMultiIndices.put(i->first, miSet);
    }

  return rcp(new FunctionalPolynomial(newFuncs, newFuncMultiIndices, newCoeffs));
}

RefCountPtr<FunctionalPolynomial> FunctionalPolynomial::
addFunction(const RefCountPtr<ScalarExpr>& u, int sign) const 
{
  RefCountPtr<FunctionalPolynomial> other = rcp(new FunctionalPolynomial(u));
  return addPoly(other.get(), sign);
}

RefCountPtr<FunctionalPolynomial> FunctionalPolynomial::
multiplyScalar(const RefCountPtr<ScalarExpr>& alpha) const 
{
  typedef Map<MultipleDeriv, RefCountPtr<ScalarExpr> > termMap;

  Array<Map<MultipleDeriv, RefCountPtr<ScalarExpr> > > newCoeffs = coeffs_;

  Expr alphaExpr = Expr::handle(alpha);

  for (unsigned int i=0; i < coeffs_.size(); i++)
    {
      for (termMap::const_iterator 
             j = coeffs_[i].begin(); j != coeffs_[i].end(); j++)
        {
          const MultipleDeriv& key = j->first;
          Expr oldCoeff = Expr::handle(j->second);
          Expr newCoeff = alphaExpr*oldCoeff;

          const ScalarExpr* se 
            = dynamic_cast<const ScalarExpr*>(newCoeff.ptr().get());
          TEST_FOR_EXCEPTION(se==0, InternalError,
                             "Coefficient could not be cast to "
                             "scalar expression");
      
          newCoeffs[i].put(key, rcp_dynamic_cast<ScalarExpr>(newCoeff.ptr()));
        }
    }
  
  return rcp(new FunctionalPolynomial(funcs_, funcMultiIndices_, newCoeffs));
}


Evaluator* FunctionalPolynomial::createEvaluator(const EvaluatableExpr* expr,
                                                 const EvalContext& context) const
{
  TEST_FOR_EXCEPTION(true, RuntimeError, "poly eval not ready");
  return (Evaluator*) 0;
}



bool FunctionalPolynomial::isConvertibleToPoly(const ScalarExpr* expr)
{
  const SymbolicFuncElement* s 
    = dynamic_cast<const SymbolicFuncElement*>(expr);
  
  const DerivOfSymbFunc* d
    = dynamic_cast<const DerivOfSymbFunc*>(expr);

  const FunctionalPolynomial* p
    = dynamic_cast<const FunctionalPolynomial*>(expr);

  return (s != 0 || d != 0 || p != 0);
}


RefCountPtr<FunctionalPolynomial> FunctionalPolynomial::toPoly(const RefCountPtr<ScalarExpr>& expr)
{
  const FunctionalPolynomial* p
    = dynamic_cast<const FunctionalPolynomial*>(expr.get());

  if (p != 0) return rcp_dynamic_cast<FunctionalPolynomial>(expr);

  Expr rtn = new FunctionalPolynomial(expr);
  return rcp_dynamic_cast<FunctionalPolynomial>(rtn.ptr());
}


ostream& FunctionalPolynomial::toText(ostream& os, bool paren) const
{
  os << evalString();
  return os;
}

ostream& FunctionalPolynomial::toLatex(ostream& os, bool paren) const
{
  os << toXML();
  return os;
}

XMLObject FunctionalPolynomial::toXML() const
{
  XMLObject rtn("Polynomial");
  for (unsigned int order=0; order<coeffs_.size(); order++)
    {
      for (Map<MultipleDeriv, RefCountPtr<ScalarExpr> >::const_iterator
             i = coeffs_[order].begin(); i != coeffs_[order].end(); i++)
        {
          const MultipleDeriv& key = i->first;
          Expr e = Expr::handle(i->second);
          XMLObject term("Term");
          XMLObject coeff("Coeff");
          coeff.addChild(e.toXML());
          term.addChild(coeff);
          for (MultipleDeriv::const_iterator
                 j = key.begin(); j != key.end(); j++)
            {
              XMLObject f("FunctionalDeriv");
              f.addAttribute("mu", j->toString());
              term.addChild(f);
            }
          rtn.addChild(term);
        }
    }
  return rtn;
}

Set<Deriv> FunctionalPolynomial
::findFuncsForSummation(const Set<MultipleDeriv>& prevSet,
                        const MultipleDeriv& currentDeriv) const
{
  Set<Deriv> rtn;

  for (Set<MultipleDeriv>::const_iterator
         i = prevSet.begin(); i != prevSet.end(); i++)
    {
      const MultipleDeriv& mdPrev = *i;
      TEST_FOR_EXCEPTION(currentDeriv.size()+1 != mdPrev.size(),
                         InternalError,
                         "deriv orders must differ by 1. Found "
                         "currentDeriv.size()=" << currentDeriv.size()
                         << " while mdPrev.size()=" << mdPrev.size());

      /* We are looking for cases where the previous multiple derivative
       * is equal to the current plus one *greater* element. In such cases, the
       * set difference will contain exactly one element, and that element
       * will be greater than or equal to the the upper bound of the current 
       * set */
      Set<Deriv> tmp;
      set_difference(mdPrev.begin(), mdPrev.end(),
                     currentDeriv.begin(), currentDeriv.end(),
                     inserter(tmp, tmp.begin()));
      if (tmp.size()==1)
        {
          const Deriv& d = *(tmp.begin());
          if (currentDeriv.upper_bound(d) == currentDeriv.end()) rtn.put(d);
        }
    }
  return rtn;
}


MultipleDeriv FunctionalPolynomial::successorTerm(const MultipleDeriv& md) const
{
  MultipleDeriv rtn;

  unsigned int k = 0;
  for (MultipleDeriv::const_iterator i=md.begin(); i!=md.end(); i++, k++)
    {
      if (k < md.size()-1) rtn.put(*i);
    }
  return rtn;
}

void FunctionalPolynomial
::stepRecurrence(int level, const Map<MultipleDeriv, string>& sPrev,
                 Map<MultipleDeriv, string>& sCurr) const 
{
  Set<MultipleDeriv> allKeys;
  Set<MultipleDeriv> inducedKeys;
  Set<MultipleDeriv> prevKeys;
  for (Map<MultipleDeriv, string>::const_iterator 
         i = sPrev.begin(); i != sPrev.end(); i++)
    {
      inducedKeys.put(successorTerm(i->first));
    }
  for (Map<MultipleDeriv, string>::const_iterator 
         i = sPrev.begin(); i != sPrev.end(); i++)
    {
      prevKeys.put(i->first);
    }

  Out::os() << "keys from prev level are " << prevKeys << endl;
  Out::os() << "induced keys are " << inducedKeys << endl;
  Out::os() << "natural keys are " << keys_[level] << endl;

  allKeys.merge(inducedKeys);
  allKeys.merge(keys_[level]);

  Out::os() << "keys active at this level are " << allKeys << endl;

  for (Set<MultipleDeriv>::const_iterator 
         i = allKeys.begin(); i != allKeys.end(); i++)
    {
      const MultipleDeriv& key = *i;
      Out::os() << "working on key " << key << endl;
      string str;
      if (coeffs_[level].containsKey(key))
        {
          str = coeffs_[level].get(key)->toString();
        }

      Set<Deriv> funcs = findFuncsForSummation(prevKeys, key);
      Out::os() << "funcs to sum are " << funcs << endl;
      for (Set<Deriv>::const_iterator 
             j = funcs.begin(); j != funcs.end(); j++)
        {
          MultipleDeriv prevKey = key;
          Out::os() << "prev key = " << prevKey << endl;
          Out::os() << "func = " << *j << endl;
          // if (key.size() > 0 && key.upper_bound(*j) == key.end()) 
          //  {
          //    Out::os() << "skipping" << endl;
          //    continue;
          // }
          prevKey.put(*j);
          TEST_FOR_EXCEPTION(!sPrev.containsKey(prevKey), InternalError,
                             "inconsisent key lookup");
          const string& prevStr = sPrev.get(prevKey);
          string funcStr = (*j).toString();
          if (str.length() > 0) str += " + ";
          str += funcStr + "*(" + prevStr + ")";
        }
      if (str.length() > 0) sCurr.put(key, str);
    }
}

string FunctionalPolynomial::evalString() const
{
  int maxLevel = coeffs_.size()-1;

  Map<MultipleDeriv, string> sPrev;
  Map<MultipleDeriv, string> sCurr;

  for (int level=maxLevel; level>=0; level--)
    {
      Out::os() << "********* Recurrence level = " << level << " ***************"
           << endl;      
      sCurr = Map<MultipleDeriv, string>();
      stepRecurrence(level, sPrev, sCurr);
      sPrev = sCurr;

      for (Map<MultipleDeriv, string>::const_iterator 
             j = sCurr.begin(); j != sCurr.end(); j++)
        {
          Out::os() << "key=" << j->first << endl;
          Out::os() << "value=" << j->second << endl;
        }
    }

  //  TEST_FOR_EXCEPTION(sCurr.size() != 1, InternalError,
  //                     "final value should have only one element");

  return sCurr.begin()->second;
}



bool FunctionalPolynomial::lessThan(const ScalarExpr* other) const
{
  const FunctionalPolynomial* f = dynamic_cast<const FunctionalPolynomial*>(other);
  TEST_FOR_EXCEPTION(f==0, InternalError, "cast should never fail at this point");
  
  TEST_FOR_EXCEPTION(true, RuntimeError, "FunctionalPolynomial::lessThan() not "
                     "implemented");
}

  