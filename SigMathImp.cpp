/*************************************************************************************

	cpl - cross-platform library - v. 0.1.0.

	Copyright (C) 2015 Janus Lynggaard Thorborg [LightBridge Studios]

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	See \licenses\ for additional details on licenses associated with this program.

**************************************************************************************

	file:SigMathImp.cpp
	
		Implementations of certain math routines (stuff relying on external linkage).

*************************************************************************************/

namespace cpl
{
	namespace Math
	{
		double chbevl(double x, double array[], int n)
		{
			double b0, b1, b2, *p;
			int i;

			p = array;
			b0 = *p++;
			b1 = 0.0;
			i = n - 1;

			do {
				b2 = b1;
				b1 = b0;
				b0 = x * b1 - b2 + *p++;
			} while (--i);

			return(0.5 * (b0 - b2));
		}

		/* Chebyshev coefficients for exp(-x) I0(x)
		* in the interval [0,8].
		*
		* lim(x->0){ exp(-x) I0(x) } = 1.
		*/

		double Chebyshev_A_Coeffs[] = {
			-4.41534164647933937950E-18,
			3.33079451882223809783E-17,
			-2.43127984654795469359E-16,
			1.71539128555513303061E-15,
			-1.16853328779934516808E-14,
			7.67618549860493561688E-14,
			-4.85644678311192946090E-13,
			2.95505266312963983461E-12,
			-1.72682629144155570723E-11,
			9.67580903537323691224E-11,
			-5.18979560163526290666E-10,
			2.65982372468238665035E-9,
			-1.30002500998624804212E-8,
			6.04699502254191894932E-8,
			-2.67079385394061173391E-7,
			1.11738753912010371815E-6,
			-4.41673835845875056359E-6,
			1.64484480707288970893E-5,
			-5.75419501008210370398E-5,
			1.88502885095841655729E-4,
			-5.76375574538582365885E-4,
			1.63947561694133579842E-3,
			-4.32430999505057594430E-3,
			1.05464603945949983183E-2,
			-2.37374148058994688156E-2,
			4.93052842396707084878E-2,
			-9.49010970480476444210E-2,
			1.71620901522208775349E-1,
			-3.04682672343198398683E-1,
			6.76795274409476084995E-1
		};



		/* Chebyshev coefficients for exp(-x) sqrt(x) I0(x)
		* in the inverted interval [8,infinity].
		*
		* lim(x->inf){ exp(-x) sqrt(x) I0(x) } = 1/sqrt(2pi).
		*/

		double Chebyshev_B_Coeffs[] = {
			-7.23318048787475395456E-18,
			-4.83050448594418207126E-18,
			4.46562142029675999901E-17,
			3.46122286769746109310E-17,
			-2.82762398051658348494E-16,
			-3.42548561967721913462E-16,
			1.77256013305652638360E-15,
			3.81168066935262242075E-15,
			-9.55484669882830764870E-15,
			-4.15056934728722208663E-14,
			1.54008621752140982691E-14,
			3.85277838274214270114E-13,
			7.18012445138366623367E-13,
			-1.79417853150680611778E-12,
			-1.32158118404477131188E-11,
			-3.14991652796324136454E-11,
			1.18891471078464383424E-11,
			4.94060238822496958910E-10,
			3.39623202570838634515E-9,
			2.26666899049817806459E-8,
			2.04891858946906374183E-7,
			2.89137052083475648297E-6,
			6.88975834691682398426E-5,
			3.36911647825569408990E-3,
			8.04490411014108831608E-1
		};
	}
};