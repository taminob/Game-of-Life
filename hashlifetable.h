// © Copyright (c) 2018 SqYtCO

#ifndef HASHLIFETABLE_H
#define HASHLIFETABLE_H

#include <cstdint>
#include <vector>

struct Macrocell;

// hash table which stores all macrocells using robin hood hashing
class HashLife_Table
{
	// store pointers to all created empty cells
	std::vector<Macrocell*> empty_cells;
	// pointers to the two states of a zero level cell
	Macrocell* alive_cell;
	Macrocell* dead_cell;
	// currently precalculated state in 2^precalced_gens_exp generations
	std::size_t precalced_gens_exp;
	// currently active rule set
	std::size_t survival_rules;
	std::size_t rebirth_rules;

	// fill hash table with macrocells up to the second level (with results)
	void fill_hash_table();
	// return next macrocell state of given second_level macrocell
	Macrocell* get_second_level_result(Macrocell* second_level);

	// clear down to last_level
	void clear_result(Macrocell* cell, std::size_t level, std::size_t last_level);

	inline bool is_empty_slot(uint8_t num) const { return (num == 0xFF); }
	// state of slots; describes distance to the actual slot (determined by hash value); 0xFF describes an empty slot
	std::vector<uint8_t> data_pos;
	// slots in which the hash values and the pointer are stored; the hash are the combined pointers of the daughter macrocells
	std::vector< std::pair<uint64_t, Macrocell*> > data;
	// amount of occupied slots
	std::size_t num_of_elements;

	// if num_of_elements / data.size() is greater than 0.5,
	static constexpr double max_load_factor = 0.5;
	// size of slots is doubled including rehashing
	void internal_resize(std::size_t new_size);

public:
	// init hash table and fill it with level 0-2 cells
	HashLife_Table();
	// delete all macrocells
	~HashLife_Table();

	// hash pointers of daughter macrocells to one hash value
	static std::size_t hash(const Macrocell* macrocell);
	static std::size_t hash(const Macrocell* nw, const Macrocell* ne, const Macrocell* se, const Macrocell* sw);

	// set current level to create empty_cells; if new_level is smaller than any previous set value, the function has no effect
	void set_level(std::size_t new_level);
	// delete all inserted macrocells and fill it with new level 0-2 macrocells
	void clear();

	// clear results of macrocells down to given level
	void clear_results(std::size_t level);

	// apply new rules; this causes a deletion of all inserted cells
	void set_rule_set(std::size_t survival_rules, std::size_t rebirth_rules);

	// return cell matching nw, ne, se and sw members of key; return nullptr if there is no match
	Macrocell* operator[](const Macrocell* key) const;
	// return cell matching given daughter cells; return nullptr if there is no match
	Macrocell* get(const Macrocell* nw, const Macrocell* ne, const Macrocell* se, const Macrocell* sw) const;

	// check if given macrocell is in stored in hash table
	inline bool count(const Macrocell* key) const
	{
		return (this->operator[](key) != nullptr);
	}
	inline bool count(const Macrocell* nw, const Macrocell* ne, const Macrocell* se, const Macrocell* sw) const
	{
		return (get(nw, ne, se, sw) != nullptr);
	}

	// insert new macrocell into hash table; value has to be != nullptr
	void insert(const Macrocell* key, Macrocell* value);
	void insert(const Macrocell* nw, const Macrocell* ne, const Macrocell* se, const Macrocell* sw, Macrocell* value);

	// set exponent (power of two) of currently precalculated generations stored in the results
	inline void set_precalced_exp(std::size_t new_precalced_exp) { precalced_gens_exp = new_precalced_exp; }

// get members
	inline Macrocell* get_empty(std::size_t level) const { return empty_cells[level]; }
	inline Macrocell* get_alive() const { return alive_cell; }
	inline Macrocell* get_dead() const { return dead_cell; }
	inline std::size_t get_precalced_exp() const{ return precalced_gens_exp; }
	inline std::size_t get_precalced() const{ return 0x01ull << precalced_gens_exp; }
};

#endif // HASHLIFETABLE_H
