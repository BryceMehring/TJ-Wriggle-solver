#ifndef _POLYNOMIALSOLVER_
#define _POLYNOMIALSOLVER_
#include <vector>

// Defines a polynomial that is sovleable
class Polynomial
{
public:

	Polynomial();
	Polynomial(double A, unsigned int B);
	~Polynomial();

	Polynomial(const Polynomial&) = delete;
	Polynomial(Polynomial&& other);

	Polynomial& operator=(const Polynomial& other) = delete;
	Polynomial& operator=(Polynomial&& other);

	// Sets the following equation form:
	// A = x + x^2 + x^3 + ... + x^B
	void SetEquation(double A, unsigned int B);

	// Returns the root specified by index
	double GetRoot(unsigned int index = 0) const;

	// Finds all the roots for the equation set
	// Returns true if the equation is solvable, false otherwise
	bool Solve();

private:

	struct PIMPL;
	PIMPL* m_pGSLWorkspace;

	std::vector<double> m_results;
	std::vector<double> m_coeffecients;
	double m_fEqual = 0.0;
	unsigned int m_iPower = 0;
};

#endif // _POLYNOMIALSOLVER_
