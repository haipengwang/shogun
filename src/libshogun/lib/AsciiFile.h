/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2010 Soeren Sonnenburg
 * Copyright (C) 2010 Berlin Institute of Technology
 */
#ifndef __ASCII_FILE_H__
#define __ASCII_FILE_H__

#include <lib/config.h>
#include <base/DynArray.h>
#include <lib/common.h>
#include <lib/File.h>
#include <lib/io.h>

namespace shogun
{
/** @brief A Ascii File access class.
 *
 * - Vectors are written out as one number per line
 * - Matrices are written out as one column per line
 * - Sparse Matrices are written out as one column per line (libsvm/svmlight
 *   style format)
 * - Strings are written out as one string per line
 *
 */
class CAsciiFile: public CFile
{
public:
	/** default constructor  */
	CAsciiFile(void);

	/** constructor
	 *
	 * @param f already opened file
	 * @param name variable name (e.g. "x" or "/path/to/x")
	 */
	CAsciiFile(FILE* f, const char* name=NULL);

	/** constructor
	 *
	 * @param fname filename to open
	 * @param rw mode, 'r' or 'w'
	 * @param name variable name (e.g. "x" or "/path/to/x")
	 */
	CAsciiFile(char* fname, char rw='r', const char* name=NULL);

	/** default destructor */
	virtual ~CAsciiFile();

	/** @name Vector Access Functions
	 *
	 * Functions to access vectors of one of the several base data types.
	 * These functions are used when loading vectors from e.g. file
	 * and return the vector and its length len by reference
	 */
	//@{
	virtual void get_byte_vector(uint8_t*& vector, int32_t& len);
	virtual void get_char_vector(char*& vector, int32_t& len);
	virtual void get_int_vector(int32_t*& vector, int32_t& len);
	virtual void get_real_vector(float64_t*& vector, int32_t& len);
	virtual void get_shortreal_vector(float32_t*& vector, int32_t& len);
	virtual void get_short_vector(int16_t*& vector, int32_t& len);
	virtual void get_word_vector(uint16_t*& vector, int32_t& len);
	//@}

	/** @name Matrix Access Functions
	 *
	 * Functions to access matrices of one of the several base data types.
	 * These functions are used when loading matrices from e.g. file
	 * and return the matrices and its dimensions num_feat and num_vec
	 * by reference
	 */
	//@{
	virtual void get_byte_matrix(
			uint8_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_int8_matrix(
			int8_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_char_matrix(
			char*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_int_matrix(
			int32_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_uint_matrix(
			uint32_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_long_matrix(
			int64_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_ulong_matrix(
			uint64_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_shortreal_matrix(
			float32_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_real_matrix(
			float64_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_longreal_matrix(
			floatmax_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_short_matrix(
			int16_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_word_matrix(
			uint16_t*& matrix, int32_t& num_feat, int32_t& num_vec);
	//@}

	/** @name N-Dimensional Array Access Functions
	 *
	 * Functions to access n-dimensional arrays of one of the several base
	 * data types. These functions are used when loading n-dimensional arrays
	 * from e.g. file and return the them and its dimensions dims and num_dims
	 * by reference
	 */
	//@{
	virtual void get_byte_ndarray(
			uint8_t*& array, int32_t*& dims, int32_t& num_dims);
	virtual void get_char_ndarray(
			char*& array, int32_t*& dims, int32_t& num_dims);
	virtual void get_int_ndarray(
			int32_t*& array, int32_t*& dims, int32_t& num_dims);
	virtual void get_shortreal_ndarray(
			float32_t*& array, int32_t*& dims, int32_t& num_dims);
	virtual void get_real_ndarray(
			float64_t*& array, int32_t*& dims, int32_t& num_dims);
	virtual void get_short_ndarray(
			int16_t*& array, int32_t*& dims, int32_t& num_dims);
	virtual void get_word_ndarray(
			uint16_t*& array, int32_t*& dims, int32_t& num_dims);
	//@}

	/** @name Sparse Matrix Access Functions
	 *
	 * Functions to access sparse matrices of one of the several base data types.
	 * These functions are used when loading sparse matrices from e.g. file
	 * and return the sparse matrices and its dimensions num_feat and num_vec
	 * by reference
	 */
	//@{
	virtual void get_bool_sparsematrix(
			TSparse<bool>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_byte_sparsematrix(
			TSparse<uint8_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_int8_sparsematrix(
			TSparse<int8_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_char_sparsematrix(
			TSparse<char>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_int_sparsematrix(
			TSparse<int32_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_uint_sparsematrix(
			TSparse<uint32_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_long_sparsematrix(
			TSparse<int64_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_ulong_sparsematrix(
			TSparse<uint64_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_short_sparsematrix(
			TSparse<int16_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_word_sparsematrix(
			TSparse<uint16_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_shortreal_sparsematrix(
			TSparse<float32_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_real_sparsematrix(
			TSparse<float64_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	virtual void get_longreal_sparsematrix(
			TSparse<floatmax_t>*& matrix, int32_t& num_feat, int32_t& num_vec);
	//@}


	/** @name String Access Functions
	 *
	 * Functions to access strings of one of the several base data types.
	 * These functions are used when loading variable length datatypes
	 * from e.g. file and return the strings and their number
	 * by reference
	 */
	//@{
	virtual void get_byte_string_list(
			TString<uint8_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_int8_string_list(
			TString<int8_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_char_string_list(
			TString<char>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_int_string_list(
			TString<int32_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_uint_string_list(
			TString<uint32_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_short_string_list(
			TString<int16_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_word_string_list(
			TString<uint16_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_long_string_list(
			TString<int64_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_ulong_string_list(
			TString<uint64_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_shortreal_string_list(
			TString<float32_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_real_string_list(
			TString<float64_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	virtual void get_longreal_string_list(
			TString<floatmax_t>*& strings, int32_t& num_str,
			int32_t& max_string_len);
	//@}

	/** @name Vector Access Functions
	 *
	 * Functions to access vectors of one of the several base data types.
	 * These functions are used when writing vectors of length len
	 * to e.g. a file
	 */
	//@{
	virtual void set_byte_vector(const uint8_t* vector, int32_t len);
	virtual void set_char_vector(const char* vector, int32_t len);
	virtual void set_int_vector(const int32_t* vector, int32_t len);
	virtual void set_shortreal_vector( const float32_t* vector, int32_t len);
	virtual void set_real_vector(const float64_t* vector, int32_t len);
	virtual void set_short_vector(const int16_t* vector, int32_t len);
	virtual void set_word_vector(const uint16_t* vector, int32_t len);
	//@}


	/** @name Matrix Access Functions
	 *
	 * Functions to access matrices of one of the several base data types.
	 * These functions are used when writing matrices of num_feat rows and
	 * num_vec columns to e.g. a file
	 */
	//@{
	virtual void set_byte_matrix(
			const uint8_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_int8_matrix(
			const int8_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_char_matrix(
			const char* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_int_matrix(
			const int32_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_uint_matrix(
			const uint32_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_long_matrix(
			const int64_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_ulong_matrix(
			const uint64_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_shortreal_matrix(
			const float32_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_real_matrix(
			const float64_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_longreal_matrix(
			const floatmax_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_short_matrix(
			const int16_t* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_word_matrix(
			const uint16_t* matrix, int32_t num_feat, int32_t num_vec);
	//@}

	/** @name Sparse Matrix Access Functions
	 *
	 * Functions to access sparse matrices of one of the several base data types.
	 * These functions are used when writing sparse matrices of num_feat rows and
	 * num_vec columns to e.g. a file
	 */
	//@{
	virtual void set_bool_sparsematrix(
			const TSparse<bool>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_byte_sparsematrix(
			const TSparse<uint8_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_int8_sparsematrix(
			const TSparse<int8_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_char_sparsematrix(
			const TSparse<char>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_int_sparsematrix(
			const TSparse<int32_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_uint_sparsematrix(
			const TSparse<uint32_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_long_sparsematrix(
			const TSparse<int64_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_ulong_sparsematrix(
			const TSparse<uint64_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_short_sparsematrix(
			const TSparse<int16_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_word_sparsematrix(
			const TSparse<uint16_t>* matrix, int32_t num_feat, int32_t num_vec); 
	virtual void set_shortreal_sparsematrix(
			const TSparse<float32_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_real_sparsematrix(
			const TSparse<float64_t>* matrix, int32_t num_feat, int32_t num_vec);
	virtual void set_longreal_sparsematrix(
			const TSparse<floatmax_t>* matrix, int32_t num_feat, int32_t num_vec);
	//@}


	/** @name String Access Functions
	 *
	 * Functions to access strings of one of the several base data types.
	 * These functions are used when writing variable length datatypes
	 * like strings to a file. Here num_str denotes the number of strings
	 * and strings is a pointer to a string structure.
	 */
	//@{
	virtual void set_byte_string_list(
			const TString<uint8_t>* strings, int32_t num_str);
	virtual void set_int8_string_list(
			const TString<int8_t>* strings, int32_t num_str);
	virtual void set_char_string_list(
			const TString<char>* strings, int32_t num_str);
	virtual void set_int_string_list(
			const TString<int32_t>* strings, int32_t num_str);
	virtual void set_uint_string_list(
			const TString<uint32_t>* strings, int32_t num_str);
	virtual void set_short_string_list(
			const TString<int16_t>* strings, int32_t num_str);
	virtual void set_word_string_list(
			const TString<uint16_t>* strings, int32_t num_str);
	virtual void set_long_string_list(
			const TString<int64_t>* strings, int32_t num_str);
	virtual void set_ulong_string_list(
			const TString<uint64_t>* strings, int32_t num_str);
	virtual void set_shortreal_string_list(
			const TString<float32_t>* strings, int32_t num_str);
	virtual void set_real_string_list(
			const TString<float64_t>* strings, int32_t num_str);
	virtual void set_longreal_string_list(
			const TString<floatmax_t>* strings, int32_t num_str);
	//@}

	/** @return object name */
	inline virtual const char* get_name() const { return "AsciiFile"; }

private:
	/** helper function to read vectors / matrices
	 *
	 * @param items dynamic array of values
	 * @param ptr_data
	 * @param ptr_item
	 */
	template <class T> void append_item(DynArray<T>* items, char* ptr_data, char* ptr_item);
};
}
#endif //__ASCII_FILE_H__
