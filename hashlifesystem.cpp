#include "hashlifesystem.h"
#include <cmath>
#include <algorithm>

// return exponent of next power of two (if num is no power of two; otherwise exponent of num)
static inline std::size_t exp_of_next_pow_of_two(std::size_t num)
{
	std::size_t exp = 0;
	--num;
	while(num >> exp)
		++exp;
	return exp;
}

// return next power of two
#if __cplusplus >= 201703L
[[maybe_unused]] static inline std::size_t next_pow_of_two(std::size_t num)
#else
static inline std::size_t next_pow_of_two(std::size_t num)
#endif
{
	--num;
	num |= (num >> 1);
	num |= (num >> 2);
	num |= (num >> 4);
	num |= (num >> 8);
	num |= (num >> 16);
	num |= (num >> 32);
	return ++num;
}

HashLife_System::HashLife_System(std::size_t survival_rules, std::size_t rebirth_rules) : Base_System(init_size, init_size), master_cell(nullptr)
{
	Macrocell::hash_table.set_rule_set(survival_rules, rebirth_rules);
	resize(init_size);
}

std::size_t HashLife_System::calc_next_generation(std::size_t generations)
{
	generations = exp_of_next_pow_of_two(generations);
	while(generations > (level - 2))
		resize(size_x * 2);

	if(generations > Macrocell::hash_table.get_precalced_exp())
		Macrocell::hash_table.clear_results(Macrocell::hash_table.get_precalced_exp() + 2);
	else if(generations < Macrocell::hash_table.get_precalced_exp())
		Macrocell::hash_table.clear_results(generations);

	auto_expand();

	master_cell->result = master_cell->calculate(level, generations);
	Macrocell::hash_table.set_precalced_exp(generations);

	return generations;
}

std::size_t HashLife_System::next_generation(std::size_t generations)
{
	calc_next_generation(generations);
	master_cell = master_cell->calculate(level, Macrocell::hash_table.get_precalced());
	Macrocell::hash_table.resize(--level);
	size_x >>= 1;
	size_y >>= 1;
	expand();
	calc_next_generation(generations);

	return Macrocell::hash_table.get_precalced();
}

void HashLife_System::resize(std::size_t size)
{
	if(size < 2)
		return;
	std::size_t prev_level = level;
	level = exp_of_next_pow_of_two(size) + 1;
	size_x = 0x01ull << (level - 1);
	size_y = size_x;

	Macrocell::hash_table.resize(level);

	if(master_cell == nullptr)
	{
		master_cell = Macrocell::hash_table.get_empty()[level];
		return;
	}
	else
	{
		Macrocell* new_nw = master_cell->nw;
		Macrocell* new_ne = master_cell->ne;
		Macrocell* new_se = master_cell->se;
		Macrocell* new_sw = master_cell->sw;
		Macrocell* empty = Macrocell::hash_table.get_empty()[level - 2];

		int change = static_cast<int>(prev_level) - static_cast<int>(level);
		while(change-- > 0)
		{
			new_nw = new_nw->se;
			new_ne = new_ne->sw;
			new_se = new_se->nw;
			new_sw = new_sw->ne;
		}

		master_cell = Macrocell::new_macrocell(Macrocell::new_macrocell(empty, empty, new_nw, empty),
									Macrocell::new_macrocell(empty, empty, empty, new_ne),
									Macrocell::new_macrocell(new_se, empty, empty, empty),
									Macrocell::new_macrocell(empty, new_sw, empty, empty));
	}
}
