/* Helper test function for speed-up (in assembly)
    Copyright (C) 2014 Victoria Rudakova <vicrucann@gmail.com>

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
*/

#ifndef ALTERNATIVES_H
#define ALTERNATIVES_H

#ifdef _WIN32
#if (!_WIN64)
inline double ddot_asm(double* v1_ptr, double* v2_ptr, int cnt) {
	double res = 0;
	__asm
	{
		push esi 
		push edi

		mov ecx, cnt
		mov esi, v1_ptr
		mov edi, v2_ptr

		_loop:
			fld qword ptr [esi]
			fld qword ptr [edi]

			fmulp st(1), st
			fld qword ptr res
			faddp st(1), st
			fstp qword ptr res
		
			add esi, 8
			add edi, 8
			dec ecx
		jnz _loop
		
		pop edi
		pop esi
	}
	return res;
}
#endif
#endif

#endif
