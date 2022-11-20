#pragma once

#ifndef GH_LIB
#include <stdbool.h>
#endif

typedef struct ini_entry_s {
    char *key;
    char *value;
} ini_entry_s;

typedef struct ini_section_s {
    char *name;
    ini_entry_s *entry;
    int size;
} ini_section_s;

typedef struct ini_table_s {
    ini_section_s *section;
    int size;
} ini_table_s;

/**
 * @brief Creates an empty ini_table_s struct for writing new entries to.
 * @return ini_table_s*
 */
ini_table_s *ini_table_create();

/**
 * @brief Free up all the allocated resources in the ini_table_s struct.
 * @param table
 */
void ini_table_destroy(ini_table_s *table);

/**
 * @brief Creates an ini_table_s struct filled with data from the specified
 *        `file'.  Returns NULL if the file can not be read.
 * @param table
 * @param file
 * @return ini_table_s*
 */
bool ini_table_read_from_file(ini_table_s *table, const char *file);

/**
 * @brief Writes the specified ini_table_s struct to the specified `file'.
 *        Returns false if the file could not be opened for writing, otherwise
 *        true.
 * @param table
 * @param file
 * @return bool
 */
bool ini_table_write_to_file(ini_table_s *table, const char *file);

/**
 * @brief Creates a new entry in the `table' containing the `key' and `value'
 *        provided if it does not exist.  Otherwise, modifies an exsiting `key'
 *        with the new `value'
 * @param table
 * @param section_name
 * @param key
 * @param value
 */
void ini_table_create_entry(ini_table_s *table, const char *section_name, const char *key, const char *value);

/**
 * @brief Checks for the existance of an entry in the specified `table'.  Returns
 *        false if the entry does not exist, otherwise true.
 * @param table
 * @param section_name
 * @param key
 * @return bool
 */
bool ini_table_check_entry(ini_table_s *table, const char *section_name, const char *key);

/**
 * @brief Retrieves the unmodified value of the specified `key' in `section_name'.
 *        Returns NULL if the entry does not exist, otherwise a pointer to the
 *        entry value data.
 * @param table
 * @param section_name
 * @param key
 * @return const char*
 */
const char *ini_table_get_entry(ini_table_s *table, const char *section_name, const char *key);

/**
 * @brief Retrieves the value of the specified `key' in `section_name', converted
 *        to int.  Returns false on failure, otherwise true.
 * @param table
 * @param section_name
 * @param key
 * @param [out]value
 * @return int
 */
bool ini_table_get_entry_as_int(ini_table_s *table, const char *section_name, const char *key, int *value);

/**
 * @brief Retrieves the value of the specified `key' in `section_name', converted
 *        to bool.  Returns false on failure, true otherwise.
 * @param table
 * @param section_name
 * @param key
 * @param [out]value
 * @return bool
 */
bool ini_table_get_entry_as_bool(ini_table_s *table, const char *section_name, const char *key, bool *value);

// Ctn: make this non-static
ini_section_s *_ini_section_find(ini_table_s *table, const char *name);
