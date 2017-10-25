#pragma once

#include <stdexcept>
#include <vector>

template<class T>
class DynamicArray
{
public:
	DynamicArray(size_t sizeX, size_t size_y, size_t size_z) :data(sizeX * size_y * size_z),
		size{ sizeX, size_y, size_z }, _indexOffset{0,0,0}
	{
	}

	size_t numElements() const {
		return size[0] * size[1] * size[2];
	}

	bool contains(int x, int y, int z) const {
		return (x - _indexOffset[0] >= 0) && (x - _indexOffset[0] < (int)size[0]) 
			&& (y - _indexOffset[1] >= 0) && (y - _indexOffset[1] < (int)size[1])
			&& (z - _indexOffset[2] >= 0) && (z - _indexOffset[2] < (int)size[2]);
	}

	size_t indexOf(int x, int y, int z) const {
		return indexOf(x - _indexOffset[0], y - _indexOffset[1], z - _indexOffset[2], size[0], size[1]);
	}

	const T& at(int x, int y, int z) const {
		checkRange(x, y, z);
		return data[indexOf(x, y, z)];
	}

	T& at(int x, int y, int z) {
		checkRange(x, y, z);
		return data[indexOf(x, y, z)];
	}

	/**
		Negative means into negative direction => also move the object
	*/
	void expand(int off_x, int off_y, int off_z) {
		size_t size_x = size[0] + std::abs(off_x);
		size_t size_y = size[1] + std::abs(off_y);
		size_t size_z = size[2] + std::abs(off_z);

		if ((size[0] == size_x) && (size[1] == size_y) && (size[2] == size_z))
			return; // has already given size

		std::vector<T> newData(size_x * size_y * size_z);

		// determine copy offset
		off_x = off_x < 0 ? -off_x : 0; // 0 if expant into positive direction, -off if we need space "in front"
		off_y = off_y < 0 ? -off_y : 0;
		off_z = off_z < 0 ? -off_z : 0;

		// copy over elements
		for (size_t x = 0; x < size[0]; x++) {
			for (size_t y = 0; y < size[1]; y++) {
				for (size_t z = 0; z < size[2]; z++) {
					newData[indexOf(x + off_x, y + off_y, z + off_z, size_x, size_y)] = this->data[indexOf(x, y, z, size[0], size[1])];
				}
			}
		}

		// take over new data
		size[0] = size_x;
		size[1] = size_y;
		size[2] = size_z;
		data = newData;
	}

	void ensureIndexExists(int x, int y, int z)
	{
		// check if block fits into data
		if (!contains(x, y, z)) {
			int off_x = computeExpandOffset(x, 0);
			int off_y = computeExpandOffset(y, 1);
			int off_z = computeExpandOffset(z, 2);

			expand(off_x, off_y, off_z);

			if (off_x < 0) {
				_indexOffset[0] += off_x;
			}

			if (off_y < 0) {
				_indexOffset[1] += off_y;
			}

			if (off_z < 0) {
				_indexOffset[2] += off_z;
			}
		}
	}

	inline size_t sizeX() const {
		return size[0];
	}

	inline size_t sizeY() const {
		return size[1];
	}

	inline size_t sizeZ() const {
		return size[2];
	}

	inline size_t sizeOfDim(size_t i) const {
		if (i >= 3)
			throw std::out_of_range("i should be < 3!");

		return size[i];
	}
	
	inline std::pair<int, int> rangeX() const {
		return std::make_pair(_indexOffset[0], _indexOffset[0]+size[0]);
	};
	
	inline std::pair<int, int> rangeY() const {
		return std::make_pair(_indexOffset[1], _indexOffset[1]+size[1]);
	};
	
	inline std::pair<int, int> rangeZ() const {
		return std::make_pair(_indexOffset[2], _indexOffset[2]+size[2]);
	};

private:
	const static int _ExpandStepSize = 3;

	std::vector<T> data;
	size_t size[3];
	int _indexOffset[3]; // negative offset

	inline static size_t indexOf(size_t x, size_t y, size_t z, size_t size_x, size_t size_y) {
		return z*size_y * size_x + y * size_x + x;
	}

	void checkRange(int x, int y, int z) const {
		if (!contains(x, y, z))
			throw std::out_of_range("Element is not in array"); // TODO: print size and index
	}

	inline int computeExpandOffset(int pos, size_t dim) {
		pos -= _indexOffset[dim];
		return pos < 0 ? pos - _ExpandStepSize : (pos >= (int)sizeOfDim(dim) ? pos - (int)sizeOfDim(dim) + 1 + _ExpandStepSize : 0);
	}
};

