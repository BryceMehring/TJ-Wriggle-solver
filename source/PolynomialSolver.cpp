#include "PolynomialSolver.h"
#include <gsl/gsl_poly.h>
#include <gsl/gsl_errno.h>

struct Polynomial::PIMPL
{
	gsl_poly_complex_workspace* w = nullptr;
};

Polynomial::Polynomial()
{
	m_pGSLWorkspace = new Polynomial::PIMPL();
}

Polynomial::Polynomial(double A, unsigned int B) : Polynomial()
{
	SetEquation(A,B);
}

Polynomial::Polynomial(Polynomial &&other) : m_pGSLWorkspace(nullptr)
{
	*this = std::move(other);
}

Polynomial& Polynomial::operator =(Polynomial&& other)
{
	delete m_pGSLWorkspace;
	m_pGSLWorkspace = other.m_pGSLWorkspace;
	other.m_pGSLWorkspace = nullptr;

	m_coeffecients = std::move(other.m_coeffecients);
	m_results = std::move(other.m_results);
	m_fEqual = other.m_fEqual;
	m_iPower = other.m_iPower;

	return *this;
}

Polynomial::~Polynomial()
{
	if(m_pGSLWorkspace != nullptr)
	{
		gsl_poly_complex_workspace_free(m_pGSLWorkspace->w);
		delete m_pGSLWorkspace;
	}
}

void Polynomial::SetEquation(double eq, unsigned int power)
{
	if((power + 1) != m_coeffecients.size())
	{
		gsl_poly_complex_workspace_free(m_pGSLWorkspace->w);
		m_pGSLWorkspace->w = nullptr;
	}

	m_fEqual = eq;
	m_iPower = power;

	m_coeffecients.resize(m_iPower + 1,1.0);
	m_results.resize(2*m_iPower);
	m_coeffecients[0] = -eq;

	if(m_pGSLWorkspace->w == nullptr)
	{
		m_pGSLWorkspace->w = gsl_poly_complex_workspace_alloc(m_coeffecients.size());

		if(m_pGSLWorkspace->w == nullptr)
		{
			throw std::bad_alloc();
		}
	}
}

double Polynomial::GetRoot(unsigned int index) const
{
	if(m_results.empty() || index >= m_results.size())
		return 0.0;

	return m_results[index];
}

bool Polynomial::Solve()
{
	if(m_coeffecients.empty())
		return false;

	int success = gsl_poly_complex_solve(m_coeffecients.data(), m_coeffecients.size(), m_pGSLWorkspace->w, m_results.data());

	return success == GSL_SUCCESS;
}
