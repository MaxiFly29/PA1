#include <iostream>
#include <array>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <optional>
#include <chrono>
#include <fstream>

constexpr std::size_t EMPTY = 0;
constexpr std::size_t fact9 = 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9;

class Logger
{
public:
	Logger() = default;

	inline std::size_t getIterationCount() const
	{
		return m_itercnt;
	}

	inline std::size_t getTime() const
	{
		return m_time;
	}

	inline std::size_t getConditionsCount() const {
		return m_condcnt;
	}

	inline std::size_t getConditionsInMemoryCount() const
	{
		return m_condinmemcnt;
	}

	inline void startTimer()
	{
		m_curtime = std::chrono::high_resolution_clock::now();
	}

	inline void stopTimer()
	{
		using namespace std::chrono;
		m_time = duration_cast<microseconds>(high_resolution_clock::now() - m_curtime).count();
	}

	inline void IncrementIterationCounter()
	{
		m_itercnt++;
	}

	inline void IncrementConditionCount()
	{
		m_condcnt++;
	}

	inline void setConditionInMemoryCount(std::size_t cnt)
	{
		m_condinmemcnt = cnt;
	}
private:
	std::size_t m_itercnt = 0, m_time = 0, m_condcnt = 0, m_condinmemcnt;
	std::chrono::time_point<std::chrono::steady_clock> m_curtime;
};

class Game8Puzzle {
public:
	Game8Puzzle() = default;
	Game8Puzzle(const std::array<std::size_t, 9>& in_matrix) :
		m_matrix(in_matrix) {
		calcPosOfEmpty();
	}

	bool moveLeft();
	bool moveRight();
	bool moveDown();
	bool moveUp();

	const std::array<std::size_t, 9>& getMatrix() const { return m_matrix; }

	std::size_t CalcManhattanDist() const;
	std::size_t CalcWrongPuzzles() const;
	bool IsSolved() const;
	bool operator==(const Game8Puzzle& r) const {
		return m_matrix == r.m_matrix;
	}

private:

	void calcPosOfEmpty() const;

	std::array<std::size_t, 9> m_matrix = { 1, 2, 3, 4, 5, 6, 7, 8, EMPTY };
	mutable std::size_t posOfEmpty = 8;
};



std::ostream& operator<<(std::ostream& out, const Game8Puzzle& game) {
	out << std::string(5, '_') << '\n';
	for (int i = 0; i < 3; i++) {
		out << '|';
		for (int j = 0; j < 3; j++) {
			out << game.getMatrix()[i * 3 + j];
			if (j < 2)
				out << ' ';
		}
		out << "|\n";
	}
	out << std::string(5, '_');
	return out;
}

template<>
struct std::hash<Game8Puzzle> {
	std::size_t operator()(const Game8Puzzle& game) const {
		std::size_t res = 0;
		const auto& matrix = game.getMatrix();
		for (int i = 0; i < matrix.size(); i++) {
			res *= 10;
			res += matrix[i];
		}
		return res;
	}
};

void Game8Puzzle::calcPosOfEmpty() const {
	for (int i = 0; i < 9; i++) {
		if (m_matrix[i] == 0) {
			posOfEmpty = i;
			return;
		}
	}
}

bool Game8Puzzle::moveLeft() {
	if (posOfEmpty % 3 != 2) {
		std::swap(m_matrix[posOfEmpty], m_matrix[posOfEmpty + 1]);
		posOfEmpty++;
		return true;
	}
	return false;
}

bool Game8Puzzle::moveRight() {
	if (posOfEmpty % 3 != 0) {
		std::swap(m_matrix[posOfEmpty], m_matrix[posOfEmpty - 1]);
		posOfEmpty--;
		return true;
	}
	return false;
}

bool Game8Puzzle::moveDown() {
	if (posOfEmpty > 2) {
		std::swap(m_matrix[posOfEmpty], m_matrix[posOfEmpty - 3]);
		posOfEmpty -= 3;
		return true;
	}
	return false;
}

bool Game8Puzzle::moveUp() {
	if (posOfEmpty < 6) {
		std::swap(m_matrix[posOfEmpty], m_matrix[posOfEmpty + 3]);
		posOfEmpty += 3;
		return true;
	}
	return false;
}

std::size_t Game8Puzzle::CalcManhattanDist() const {
	std::size_t res = 0;
	for (int i = 0; i < 9; i++) {
		int cur = (m_matrix[i] + 8) % 9;
		res += std::abs(cur % 3 - i % 3) + std::abs(cur / 3 - i / 3);
	}
	return res;
}

std::size_t Game8Puzzle::CalcWrongPuzzles() const {
	std::size_t res = 0;
	for (int i = 0; i < 9; i++) {
		if (i != (m_matrix[i] + 8) % 9)
			res++;
	}
	return res;
}

bool Game8Puzzle::IsSolved() const {
	for (int i = 0; i < 9; i++) {
		if (i != (m_matrix[i] + 8) % 9)
			return false;
	}
	return true;
}

std::optional<std::size_t> bfs(const Game8Puzzle& game, Logger& logger) {
	logger.startTimer();
	std::size_t res = 0;
	std::queue<Game8Puzzle> q;
	std::unordered_set<Game8Puzzle> used;
	used.reserve(fact9);
	q.push(game);
	while (!q.empty()) {
		for (int i = 0, n = q.size(); i < n; i++) {
			logger.IncrementConditionCount();
			auto& cur_game = q.front();
			if (used.count(cur_game)) {
				q.pop();
				continue;
			}
			//std::cout << cur_game << '\n';
			if (cur_game.IsSolved()) {
				logger.stopTimer();
				logger.setConditionInMemoryCount(used.size());
				return res;
			}

			used.insert(cur_game);

			if (cur_game.moveRight()) {
				q.push(cur_game);
				cur_game.moveLeft();
			}
			if (cur_game.moveLeft()) {
				q.push(cur_game);
				cur_game.moveRight();
			}
			if (cur_game.moveUp()) {
				q.push(cur_game);
				cur_game.moveDown();
			}
			if (cur_game.moveDown()) {
				q.push(cur_game);
			}
			q.pop();
			logger.IncrementIterationCounter();
		}
		res++;
	}
	logger.stopTimer();
	logger.setConditionInMemoryCount(used.size());
	return std::nullopt;
}

std::optional<std::size_t> AStar(const Game8Puzzle& game, Logger& logger) {
	logger.startTimer();
	std::unordered_map<Game8Puzzle, std::size_t> dist;
	dist.reserve(fact9);
	std::multimap<std::size_t, std::unordered_map<Game8Puzzle, std::size_t>::iterator> gamesSortedByF;
	gamesSortedByF.insert({ game.CalcWrongPuzzles(), dist.insert({game, 0}).first });
	while (!gamesSortedByF.empty()) {
		logger.IncrementConditionCount();
		auto cur = gamesSortedByF.begin();
		Game8Puzzle cur_game = cur->second->first;
		std::size_t cur_dist = cur->second->second;
		gamesSortedByF.erase(cur);

		if (cur_game.IsSolved()) {
			logger.stopTimer();
			logger.setConditionInMemoryCount(dist.size());
			return cur_dist;
		}

		if (cur_game.moveRight()) {
			if (auto it = dist.find(cur_game); it == dist.end() || it->second > cur_dist + 1) {
				if (it == dist.end())
					it = dist.insert({ cur_game, cur_dist + 1 }).first;
				else
					it->second = cur_dist + 1;
				gamesSortedByF.insert({ cur_game.CalcWrongPuzzles() + cur_dist + 1, it });
			}
			cur_game.moveLeft();
		}
		if (cur_game.moveLeft()) {
			if (auto it = dist.find(cur_game); it == dist.end() || it->second > cur_dist + 1) {
				if (it == dist.end())
					it = dist.insert({ cur_game, cur_dist + 1 }).first;
				else
					it->second = cur_dist + 1;
				gamesSortedByF.insert({ cur_game.CalcWrongPuzzles() + cur_dist + 1, it });
			}
			cur_game.moveRight();
		}
		if (cur_game.moveUp()) {
			if (auto it = dist.find(cur_game); it == dist.end() || it->second > cur_dist + 1) {
				if (it == dist.end())
					it = dist.insert({ cur_game, cur_dist + 1 }).first;
				else
					it->second = cur_dist + 1;
				gamesSortedByF.insert({ cur_game.CalcWrongPuzzles() + cur_dist + 1, it });
			}
			cur_game.moveDown();
		}
		if (cur_game.moveDown()) {
			if (auto it = dist.find(cur_game); it == dist.end() || it->second > cur_dist + 1) {
				if (it == dist.end())
					it = dist.insert({ cur_game, cur_dist + 1 }).first;
				else
					it->second = cur_dist + 1;
				gamesSortedByF.insert({ cur_game.CalcWrongPuzzles() + cur_dist + 1, it });
			}
		}
		logger.IncrementIterationCounter();
	}
	logger.stopTimer();
	logger.setConditionInMemoryCount(dist.size());
	return std::nullopt;
}

int main()
{
	std::ofstream out("res_table.txt");
	std::array<std::size_t, 9> matrix({ 2,1,4,
									  6,7,3,
									  5,8,EMPTY });
	for (int i = 0; i < 20; i++) {
		Game8Puzzle game(matrix);
		std::optional<std::size_t> sol1, sol2;
		Logger logger_bfs, logger_astar;

		sol1 = bfs(game, logger_bfs);

		sol2 = AStar(game, logger_astar);

		if (sol1 != sol2) {
			std::cerr << *sol1 << ' ' << *sol2 << " error!\n";
		}
		out << i + 1 << " condition\t" << logger_bfs.getIterationCount() << '\t' << !sol1.has_value() << '\t'
			<< logger_bfs.getConditionsCount() << '\t' << logger_bfs.getConditionsInMemoryCount() << '\t' << logger_bfs.getTime() / 1e3 << "ms\t\t"
			<< logger_astar.getIterationCount() << '\t' << !sol2.has_value() << '\t'
			<< logger_astar.getConditionsCount() << '\t' << logger_astar.getConditionsInMemoryCount() << '\t' << logger_astar.getTime() / 1e3 << "ms\n";
		for (int j = 0, n = rand(); j < n; j++)
			std::next_permutation(matrix.begin(), matrix.end());
	}
}