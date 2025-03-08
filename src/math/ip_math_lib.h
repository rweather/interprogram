/*
 * Copyright (C) 2025 Rhys Weatherley
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef INTERPROGRAM_MATH_LIB_H
#define INTERPROGRAM_MATH_LIB_H

#include "ip_exec.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forms the absolute value of "THIS"
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_abs(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the sine of "THIS", where the input value is in
 * multiples of pi.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_sin_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the sine of "THIS", where the input value is in radians.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_sin_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the sine of "THIS", where the input value is in degrees.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_sin_degrees(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the cosine of "THIS", where the input value is in
 * multiples of pi.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_cos_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the cosine of "THIS", where the input value is in radians.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_cos_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the cosine of "THIS", where the input value is in degrees.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_cos_degrees(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the tangent of "THIS", where the input value is in
 * multiples of pi.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_tan_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the tangent of "THIS", where the input value is in radians.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_tan_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the tangent of "THIS", where the input value is in degrees.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_tan_degrees(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the arctangent of "THIS", where the result is in
 * multiples of pi.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_atan_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the arctangent of "THIS", where the result is in radians.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_atan_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the arctangent of "THIS", where the result is in degrees.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_atan_degrees(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the square root of "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_sqrt(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Forms the natural logarithm of "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_log(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Raises e to the power of "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_exp(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Raises "THIS" to the power of the first argument.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_pow(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Rounds "THIS" to the nearest whole number.  Rounds half-way
 * cases away from zero.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_round_nearest(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Rounds "THIS" down to the nearest whole number.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_round_down(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Rounds "THIS" up to the nearest whole number.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_round_up(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Performs a bitwise AND of "THIS" and the argument.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_bitwise_and(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Performs a bitwise AND of "THIS" and the NOT of the argument.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_bitwise_and_not(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Performs a bitwise OR of "THIS" and the argument.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_bitwise_or(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Performs a bitwise XOR of "THIS" and the argument.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_bitwise_xor(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Performs a bitwise NOT of "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_bitwise_not(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Performs a shift left of "THIS" by the argument number of bits.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_shift_left(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Performs a shift right of "THIS" by the argument number of bits.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_shift_right(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Generate a random number in "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_rand(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Seed the random number.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_srand(ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Registers the math built-ins in this library with a program.
 *
 * @param[in,out] program The program state.
 * @param[in] options Options that indicate if classic or extension
 * built-ins are required.
 */
void ip_register_math_builtins(ip_program_t *program, unsigned options);

#ifdef __cplusplus
}
#endif

#endif
