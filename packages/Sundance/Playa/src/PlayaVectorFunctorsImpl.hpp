/* @HEADER@ */
//   
 /* @HEADER@ */

#ifndef PLAYA_VECTORFUNCTORSIMPL_HPP
#define PLAYA_VECTORFUNCTORSIMPL_HPP


#include "PlayaDefs.hpp"
#include "PlayaVectorFunctorsDecl.hpp"
#include "Teuchos_MPIComm.hpp"
#include "PlayaRand.hpp"

namespace PlayaFunctors
{

/** \brief Elementwise absolute value */
template <class Scalar>
class Abs
{
public:
  /** */
  Abs() {}

  /** */
  Scalar operator()(const Scalar& x) const 
    {return ::fabs(x);}

  /** */
  std::string description() const {return "Abs()";}
};

/** \brief Elementwise reciprocal */
template <class Scalar>
class Reciprocal
{
public:
  /** */
  Reciprocal() {}

  /** */
  Scalar operator()(const Scalar& x) const 
    {return 1.0/x;}

  /** */
  std::string description() const {return "Reciprocal()";}
};

/** \brief Set each element to a random scalar */
template <class Scalar>
class Random
{
public:
  /** */
  Random() {}

  /** */
  Scalar operator()(const Scalar& x) const 
    {return Playa::Rand::val();}

  /** */
  std::string description() const {return "Random()";}
};

/** \brief Multiplication by a scalar */
template <class Scalar>
class ScalarMult
{
public:
  /** */
  ScalarMult(const Scalar& alpha) : alpha_(alpha) {}

  /** */
  Scalar operator()(const Scalar& x) const 
    {return alpha_*x;}

  /** */
  std::string description() const 
    {
      std::ostringstream oss;
      oss << "ScalarMult(alpha=" << alpha_ << ")";
      return oss.str();
    }
private:
  Scalar alpha_;
};

/** \brief Identity functor, used for copying */
template <class Scalar>
class Identity
{
public:
  /** */
  Identity() {}

  /** */
  Scalar operator()(const Scalar& x) const 
    {return x;}

  /** */
  std::string description() const {return "Identity()";}
};

/** \brief Setting all elements to a constant */
template <class Scalar>
class SetConstant
{
public:
  /** */
  SetConstant(const Scalar& alpha) : alpha_(alpha) {}

  /** */
  Scalar operator()(const Scalar& x) const 
    {return alpha_;}

  /** */
  std::string description() const 
    {
      std::ostringstream oss;
      oss << "SetConstant(alpha=" << alpha_ << ")";
      return oss.str();
    }
private:
  Scalar alpha_;
};


/** \brief Elementwise product (matlab dot star) */
template <class Scalar>
class DotStar
{
public:
  /** */
  DotStar() {}

  /** */
  Scalar operator()(const Scalar& x, const Scalar& y) const 
    {return x*y;}

  /** */
  std::string description() const {return "dotStar()";}
};


/** \brief Elementwise quotient (matlab dot slash) */
template <class Scalar>
class DotSlash
{
public:
  /** */
  DotSlash() {}

  /** */
  Scalar operator()(const Scalar& x, const Scalar& y) const 
    {return x/y;}

  /** */
  std::string description() const {return "dotSlash()";}
};

/** \brief x + b*y */
template <class Scalar>
class XPlusBetaY
{
public:
  /** */
  XPlusBetaY(const Scalar& b) : b_(b) {}

  /** */
  Scalar operator()(const Scalar& x, const Scalar& y) const 
    {return x + b_*y;}

  /** */
  std::string description() const 
    {
      std::ostringstream oss;
      oss << "XPlusBetaY(b=" << b_ << ")";
      return oss.str();
    }
private:
  Scalar b_;
};

/** \brief Linear combination of two vectors */
template <class Scalar>
class LC2
{
public:
  /** */
  LC2(const Scalar& a, const Scalar& b) : a_(a), b_(b) {}

  /** */
  Scalar operator()(const Scalar& x, const Scalar& y) const 
    {return a_*x + b_*y;}

  /** */
  std::string description() const 
    {
      std::ostringstream oss;
      oss << "LC2(a=" << a_ << ", b=" << b_ << ")";
      return oss.str();
    }
private:
  Scalar a_;
  Scalar b_;
};


/** \brief Linear combination of three vectors */
template <class Scalar>
class LC3
{
public:
  /** */
  LC3(const Scalar& a, const Scalar& b, const Scalar& c)
    : a_(a), b_(b), c_(c) {}

  /** */
  Scalar operator()(const Scalar& x, const Scalar& y, const Scalar& z) const 
    {return a_*x + b_*y + c_*z;}


  /** */
  std::string description() const 
    {
      std::ostringstream oss;
      oss << "LC3(a=" << a_ << ", b=" << b_ << ", c=" << c_ << ")";
      return oss.str();
    }
private:
  Scalar a_;
  Scalar b_;
  Scalar c_;
};


/** \brief Euclidean norm of a vector */
template <class Scalar>
class Norm2 : public ReductionFunctorBase<Scalar>
{
public:
  Norm2(const MPIComm& comm)
    : ReductionFunctorBase<Scalar>(comm), val_(0.0) {}

  void step(int i, const Scalar& x) const 
    {
      val_ += x*x;
    }

  void postProc() const 
    {
      Scalar final = val_;
      this->comm().allReduce(&val_, &final, 1, MPIComm::DOUBLE, MPIComm::SUM);
      val_ = final;
    }

  Scalar result() const 
    {
      return ::sqrt(val_);
    }

  /** */
  std::string description() const {return "Norm2()";}

private:
  mutable Scalar val_;
};

/** \brief Weighted 2-norm of a vector */
template <class Scalar>
class WeightedNorm2 : public ReductionFunctorBase<Scalar>
{
public:
  WeightedNorm2(const MPIComm& comm)
    : ReductionFunctorBase<Scalar>(comm), val_(0.0) {}

  void step(int i, const Scalar& x, const Scalar& y) const 
    {
      val_ += y*x*x;
    }

  void postProc() const 
    {
      Scalar final = val_;
      this->comm().allReduce(&val_, &final, 1, MPIComm::DOUBLE, MPIComm::SUM);
      val_ = final;
    }

  Scalar result() const 
    {
      return ::sqrt(val_);
    }

  /** */
  std::string description() const {return "WeightedNorm2()";}

private:
  MPIComm comm_;
  mutable Scalar val_;
};

/** \brief 1-norm of a vector */
template <class Scalar>
class Norm1 : public ReductionFunctorBase<Scalar>
{
public:
  Norm1(const MPIComm& comm)
    : ReductionFunctorBase<Scalar>(comm), val_(0.0) {}

  void step(int i, const Scalar& x) const 
    {
      val_ += ::fabs(x);
    }

  void postProc() const 
    {
      Scalar final = val_;
      this->comm().allReduce(&val_, &final, 1, MPIComm::DOUBLE, MPIComm::SUM);
      val_ = final;
    }

  Scalar result() const 
    {
      return val_;
    }

  /** */
  std::string description() const {return "Norm1()";}

private:
  mutable Scalar val_;
};

/** \brief Infinity norm of a vector */
template <class Scalar>
class NormInf : public ReductionFunctorBase<Scalar>
{
public:
  NormInf(const MPIComm& comm)
    : ReductionFunctorBase<Scalar>(comm), val_(-1.0) {}

  void step(int i, const Scalar& x) const 
    {
      Scalar z = ::fabs(x);
      if (z > val_) val_ = z;
    }

  void postProc() const 
    {
      Scalar final = val_;
      this->comm().allReduce(&val_, &final, 1, MPIComm::DOUBLE, MPIComm::MAX);
      val_ = final;
    }

  Scalar result() const 
    {
      return val_;
    }

  /** */
  std::string description() const {return "NormInf()";}

private:
  mutable Scalar val_;
};

/** \brief Dot product of two vectors */
template <class Scalar>
class DotProduct : public ReductionFunctorBase<Scalar>
{
public:
  DotProduct(const MPIComm& comm)
    : ReductionFunctorBase<Scalar>(comm), val_(0.0) {}

  void step(int i, const Scalar& x, const Scalar& y) const 
    {
      val_ += x*y;
    }

  void postProc() const 
    {
      Scalar final = val_;
      this->comm().allReduce(&val_, &final, 1, MPIComm::DOUBLE, MPIComm::SUM);
      val_ = final;
    }

  Scalar result() const 
    {
      return val_;
    }

  /** */
  std::string description() const {return "DotProduct()";}

private:
  mutable Scalar val_;
};


/** \brief Find value of minimum element of a vector */
template <class Scalar>
class Min : public ReductionFunctorBase<Scalar>
{
public:
  Min(const MPIComm& comm)
    : ReductionFunctorBase<Scalar>(comm), val_(HUGE_VAL) {}

  void step(int i, const Scalar& x) const 
    {
      if (x < val_) val_ = x;
    }

  void postProc() const 
    {
      Scalar final = val_;
      this->comm().allReduce(&val_, &final, 1, MPIComm::DOUBLE, MPIComm::MIN);
      val_ = final;
    }

  Scalar result() const 
    {
      return val_;
    }

  /** */
  std::string description() const {return "Min()";}

private:
  mutable Scalar val_;
};


/** \brief Find value of minimum element of a vector */
template <class Scalar>
class Max : public ReductionFunctorBase<Scalar>
{
public:
  Max(const MPIComm& comm)
    : ReductionFunctorBase<Scalar>(comm), val_(-HUGE_VAL) {}

  void step(int i, const Scalar& x) const 
    {
      if (x > val_) val_ = x;
    }

  void postProc() const 
    {
      Scalar final = val_;
      this->comm().allReduce(&val_, &final, 1, MPIComm::DOUBLE, MPIComm::MAX);
      val_ = final;
    }

  Scalar result() const 
    {
      return val_;
    }

  /** */
  std::string description() const {return "Max()";}

private:
  mutable Scalar val_;
};




}


#endif



