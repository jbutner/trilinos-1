
#include "Simple2DTpetraModelEvaluator.hpp"

#include "Teuchos_UnitTestHarness.hpp"


namespace {


using Teuchos::null;
using Teuchos::RCP;
typedef Thyra::ModelEvaluatorBase MEB;


//
// Unit tests
//


TEUCHOS_UNIT_TEST_TEMPLATE_1_DECL( Simple2DTpetraModelEvaluator, construct, Scalar )
{
  RCP<Simple2DTpetraModelEvaluator<Scalar> > model = simple2DTpetraModelEvaluator<Scalar>();
  TEST_ASSERT(model != null);
  TEST_EQUALITY(model->Np(), 0);
  TEST_EQUALITY(model->Ng(), 0);
  TEST_ASSERT(model->get_x_space() != null);
  TEST_EQUALITY(model->get_x_space()->dim(), 2);
  TEST_ASSERT(model->get_f_space() != null);
  TEST_EQUALITY(model->get_f_space()->dim(), 2);
  TEST_ASSERT(nonnull(model->getNominalValues().get_x()));
  TEST_ASSERT(model->create_W_op() != null);
  //TEST_ASSERT(model->get_W_factory() != null);
  MEB::InArgs<Scalar> inArgs = model->createInArgs();
  TEST_ASSERT(inArgs.supports(MEB::IN_ARG_x));
  TEST_EQUALITY(inArgs.Np(), 0);
}

TEUCHOS_UNIT_TEST_TEMPLATE_1_INSTANT_REAL_SCALAR_TYPES(
  Simple2DTpetraModelEvaluator, construct )


TEUCHOS_UNIT_TEST_TEMPLATE_1_DECL( Simple2DTpetraModelEvaluator, eval, Scalar )
{

  using Teuchos::tuple;
  using Teuchos::ArrayRCP;
  using Teuchos::as;
  using Teuchos::rcp_dynamic_cast;
  typedef Teuchos::ScalarTraits<Scalar> ST;
  typedef typename ST::magnitudeType ScalarMag;
  typedef Teuchos::ScalarTraits<ScalarMag> SMT;
  using Thyra::VectorBase;
  using Thyra::LinearOpBase;
  typedef Thyra::ModelEvaluatorBase MEB;
  typedef Thyra::TpetraOperatorVectorExtraction<Scalar,int> ConverterT;

  RCP<Simple2DTpetraModelEvaluator<Scalar> > model = simple2DTpetraModelEvaluator<Scalar>();

  const Scalar d = 7.0;
  model->set_d(d);

  const Scalar p_0 = 2.0, p_1 = 6.0;
  model->set_p(tuple<Scalar>(p_0, p_1));

  const Scalar x_0 = 3.0, x_1 =  4.0;
  model->set_x0(tuple<Scalar>(x_0, x_1));

  MEB::InArgs<Scalar> inArgs = model->getNominalValues();
  MEB::OutArgs<Scalar> outArgs = model->createOutArgs();
  const RCP<VectorBase<Scalar> > f = createMember(model->get_f_space());
  const RCP<LinearOpBase<Scalar> > W_op = model->create_W_op();
  outArgs.set_f(f);
  outArgs.set_W_op(W_op);
  model->evalModel(inArgs, outArgs);

  const ScalarMag tol = 100.0 * SMT::eps();
  
  const RCP<const Tpetra::Vector<Scalar,int> > f_tpetra = 
    ConverterT::getConstTpetraVector(f);

  const ArrayRCP<const Scalar> f_tpetra_vals = f_tpetra->get1dView();
  TEST_FLOATING_EQUALITY(f_tpetra_vals[0], as<Scalar>(x_0 + x_1*x_1 - p_0), tol);
  TEST_FLOATING_EQUALITY(f_tpetra_vals[1], as<Scalar>(d * (x_0*x_0 - x_1 - p_1)), tol);

  const RCP<const Tpetra::CrsMatrix<Scalar,int> > W_tpetra = 
    rcp_dynamic_cast<Tpetra::CrsMatrix<Scalar,int> >(
      ConverterT::getTpetraOperator(W_op));

  ArrayRCP<const int> row_indices;
  ArrayRCP<const Scalar> row_values;

  W_tpetra->getLocalRowView(0, row_indices, row_values);
  TEST_EQUALITY( row_indices[0], 0 );
  TEST_EQUALITY( row_indices[1], 1 );
  TEST_FLOATING_EQUALITY( row_values[0], as<Scalar>(1.0), tol );
  TEST_FLOATING_EQUALITY( row_values[1], as<Scalar>(2.0*x_1), tol );

  W_tpetra->getLocalRowView(1, row_indices, row_values);
  TEST_EQUALITY( row_indices[0], 0 );
  TEST_EQUALITY( row_indices[1], 1 );
  TEST_FLOATING_EQUALITY( row_values[0], as<Scalar>(d*2.0*x_0), tol );
  TEST_FLOATING_EQUALITY( row_values[1], as<Scalar>(-d), tol );

}

TEUCHOS_UNIT_TEST_TEMPLATE_1_INSTANT_REAL_SCALAR_TYPES(
  Simple2DTpetraModelEvaluator, eval )


} // namespace
