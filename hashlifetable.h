// © Copyright (c) 2018 SqYtCO

#ifndef HASHLIFETABLE_H
#define HASHLIFETABLE_H

#include <cstdint>
#include <vector>

struct Macrocell;

// hash map which stores all macrocells using robin hood hashing
class HashLife_Table
{
	std::vector<Macrocell*> empty_cells;
	Macrocell* alive_cell;
	Macrocell* dead_cell;
	std::size_t precalced_gens_exp;
	std::size_t survival_rules;
	std::size_t rebirth_rules;

	void fill_hash_table();
	Macrocell* get_second_level_result(Macrocell* second_level);
	Macrocell* calc_gen(std::size_t bitmask);

	// clear down to last_level
	void clear_result(Macrocell* cell, std::size_t level, std::size_t last_level);

	static constexpr double max_load_factor = 0.5;

	inline bool is_empty_slot(uint8_t num) const { return (num == 0xFF); }

	std::vector<uint8_t> data_pos;
	std::vector< std::pair<uint64_t, Macrocell*> > data;
	std::size_t num_of_elements;

	void internal_resize(std::size_t new_size);

public:
	HashLife_Table();
	~HashLife_Table();

	static std::size_t hash(const Macrocell* macrocell);
	static std::size_t hash(const Macrocell* nw, const Macrocell* ne, const Macrocell* se, const Macrocell* sw);

	void set_level(std::size_t new_level);
	void clear();

	void clear_results(std::size_t level);

	void set_rule_set(std::size_t survival_rules, std::size_t rebirth_rules);

	// return cell matching nw, ne, se and sw members of key; return nullptr if there is no match
	Macrocell* operator[](const Macrocell* key) const;
	// return cell matching key0(nw), key1(ne), key2(se) and key3(sw); return nullptr if there is no match
	Macrocell* get(const Macrocell* key0, const Macrocell* key1, const Macrocell* key2, const Macrocell* key3) const;

	inline bool count(const Macrocell* key) const
	{
		return (this->operator[](key) != nullptr);
	}
	inline bool count(const Macrocell* key0, const Macrocell* key1, const Macrocell* key2, const Macrocell* key3) const
	{
		return (get(key0, key1, key2, key3) != nullptr);
	}

	// value has to be != nullptr
	void insert(const Macrocell* key, Macrocell* value);
	void insert(const Macrocell* key0, const Macrocell* key1, const Macrocell* key2, const Macrocell* key3, Macrocell* value);

	inline void set_precalced_exp(std::size_t new_precalced_exp) { precalced_gens_exp = new_precalced_exp; }

	inline const std::vector<Macrocell*>& get_empty() const { return empty_cells; }
	inline Macrocell* get_alive() const { return alive_cell; }
	inline Macrocell* get_dead() const { return dead_cell; }
	inline std::size_t get_precalced_exp() const{ return precalced_gens_exp; }
	inline std::size_t get_precalced() const{ return 0x01ull << precalced_gens_exp; }
};

#endif // HASHLIFETABLE_H
