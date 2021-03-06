/*
 * Copyright (c) 2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#if !defined(WIN64)
#include "mthdecls.h"
#else
/* Ensure that the builtin definition does not generate a recursive call.
 * TODO: cleanup the code in ../src-amd so that the version there can
 * be compiled with pgcc (rather than just gcc).
 */
double atan(double d);
#endif
double
__mth_i_datan(double d)
{
  return atan(d);
}
