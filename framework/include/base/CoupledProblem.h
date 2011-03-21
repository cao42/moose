#ifndef COUPLEDPROBLEM_H_
#define COUPLEDPROBLEM_H_

#include <vector>
#include <string>
#include <map>

#include "Problem.h"
#include "ParallelUniqueId.h"
#include "InputParameters.h"
#include "Function.h"
// libMesh
#include "equation_systems.h"
#include "numeric_vector.h"
#include "sparse_matrix.h"
#include "transient_system.h"
#include "nonlinear_implicit_system.h"

namespace Moose
{

class SubProblem;
class Variable;
class Mesh;

class CoupledProblem : public Moose::Problem
{
public:
  CoupledProblem(Mesh * mesh);
  virtual ~CoupledProblem();

  virtual EquationSystems & es() { return _eq; }
  Mesh & mesh() { return *_mesh; }

  virtual Problem * parent() { return NULL; }

  void addSubProblem(const std::string & file_name, SubProblem *subproblem);
  SubProblem *subProblem(const std::string & name);

  void solveOrder(const std::vector<std::string> & solve_order);

  // API /////
  virtual bool hasVariable(const std::string & var_name);
  virtual Variable & getVariable(THREAD_ID tid, const std::string & var_name);

  virtual void attachQuadratureRule(QBase *qrule, THREAD_ID tid);
  virtual void reinitElem(const Elem * elem, THREAD_ID tid);
  virtual void reinitElemFace(const Elem * elem, unsigned int side, unsigned int bnd_id, THREAD_ID tid);
  virtual void reinitNode(const Node * node, THREAD_ID tid);
  virtual void reinitNodeFace(const Node * node, unsigned int bnd_id, THREAD_ID tid);

  // Solve /////
  virtual void init();
  virtual void update();

  virtual void computeResidual(NonlinearImplicitSystem & sys, const NumericVector<Number> & soln, NumericVector<Number> & residual);
  virtual void computeJacobian(NonlinearImplicitSystem & sys, const NumericVector<Number> & soln, SparseMatrix<Number> &  jacobian);

  // Initial conditions /////
  virtual Number initialValue (const Point & p, const Parameters & parameters, const std::string & sys_name, const std::string & var_name);
  virtual Gradient initialGradient (const Point & p, const Parameters & parameters, const std::string & sys_name, const std::string & var_name);

  virtual void initialCondition(EquationSystems & es, const std::string & system_name);

  // Transient /////
  virtual void transient(bool trans);
  virtual bool transient() { return _transient; }

  virtual Real & time() { return _time; }
  virtual int & timeStep() { return _t_step; }
  virtual Real & dt() { return _dt; }
  virtual Real & dtOld() { return _dt_old; }

  virtual void copySolutionsBackwards();

  virtual void dump();

protected:
  std::map<std::string, Moose::SubProblem *> _subproblems;
  std::vector<std::string> _solve_order;

  /// Keep track of the correspondence between libMesh objects and Moose objects
  std::map<std::string, Moose::SubProblem *> _map;

  Mesh * _mesh;
  EquationSystems _eq;

  bool _transient;
  Real & _time;
  int & _t_step;
  Real & _dt;
  Real _dt_old;
};

}

#endif /* COUPLEDPROBLEM_H_ */
