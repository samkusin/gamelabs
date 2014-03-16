/**
 * \file    overview/grid.hpp
 *
 * A 2d grid data object for tile-maps and other MxN data structures.
 * 
 * \note    Created by Samir Sinha on 3/10/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_GRID_HPP
#define CINEK_OVERVIEW_GRID_HPP

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <iterator>
#include <algorithm>

namespace cinekine {
    namespace overview {

/**
 * @class Grid grid.hpp "cinek/overview/grid.hpp"
 * @brief Defines a 2D grid of data of type T.
 */
template<typename Value>
class Grid
{   
public:
    /** 
     *  Defines a start and end point for T items in a row.  The end pointer
     *  points past the last item in the strip (iteration should stop if the compared
     *  pointer is greater than or equal to the end pointer.
     */
    typedef std::pair<Value*, Value*> row_strip;
    /** Const version of a row_strip */
    typedef std::pair<const Value*, const Value*> const_row_strip;

    Grid(const Grid& ) = delete;
    Grid& operator=(const Grid& ) = delete;

    /**
     * Empty grid constructor.
     */
    Grid();
    /** 
     *  Constructor that creates an gridwith specified dimensions.
     *  
     *  @param rowCount     Number of rows in the grid.
     *  @param columnCount  Number of columns in the grid.
     */
    Grid(uint32_t rowCount, uint32_t columnCount);
    /**
     *  Move constructor.
     *  @param grid The grid being moved into this object.
     */
    Grid(Grid&& grid) noexcept;
    /**
     * Destructor.
     */
    ~Grid();
    /**
     * Assignment move operator.
     */
    Grid& operator=(Grid&& grid) noexcept;

    /** @return Number of rows in grid. */
    uint32_t rowCount() const { return _rowCount; }
    /** @return Number of columns in grid */
    uint32_t columnCount() const { return _colCount; }
    /**
     * Gets a const reference of the value at the specified row, column. 
     * @param  row The row [0, rowCount-1]
     * @param  col The column [0, columnCount-1]
     * @return     A reference of the value at row,col.
     */
    const Value& at(uint32_t row, uint32_t col) const;
    /**
     * Gets a reference of the value at the specified row, column. 
     * @param  row The row [0, rowCount-1]
     * @param  col The column [0, columnCount-1]
     * @return     A reference of the value at row,col.
     */
    Value& at(uint32_t row, uint32_t col);
    /**
     * Returns a pointer to the data row in the grid.  To retrieve data at a 
     * specific column, use pointer math, atRow(row) + columnIndex.
     * @param  row      Row index in grid [0, rowCount()]
     * @param  col      Column offset from start of row.
     * @param  length   Number of columns in row to use.  If UINT32_MAX, the strip
     *                  will run to the end of the row.
     * @return          A row strip pointer pair.  If the row or column 
     *                  parameter lie out of the grid's range, returns a 
     *                  null_row_strip.
     */
    row_strip atRow(uint32_t row, uint32_t col, uint32_t length=UINT32_MAX);
    /**
     * Returns a const pointer to the data row in the grid.  To retrieve data 
     * at a specific column, use pointer math, atRow(row) + columnIndex.
     * @param  row Row index in grid [0, rowCount()]
     * @param  col      Column offset from start of row.
     * @param  length   Number of columns in row to use.  If UINT32_MAX, the strip
     *                  will run to the end of the row.
     * @return          A row strip pointer pair.  If the row or column 
     *                  parameter lie out of the grid's range, returns a 
     *                  null_row_strip.
     */
    const_row_strip atRow(uint32_t row, uint32_t col, uint32_t length=UINT32_MAX) const;
    /**
     * Copies a portion of this grid to another grid (the target.)  Clipping is
     * supported if part of the source rectangle resides outside the target grid.
     * @param targetGrid    Copies the source grid data from the specified 
     *                      row,col,width and height to this grid.
     * @param sourceRow     The row index to copy from.
     * @param sourceCol     The column index to copy from.
     * @param targetRow     The row index to copy to (the target.)
     * @param targetCol     The column index to copy to (the target.)
     * @param rowCount      Number of rows to copy.
     * @param colCount      Number of columns to copy.   

    void copyTo(Grid& targetGrid, uint32_t sourceRow, uint32_t sourceCol,
            uint32_t targetRow, uint32_t targetCol,
            uint32_t rowCount, uint32_t colCount);
     */

private:
    Value* _data;
    uint32_t _rowCount;
    uint32_t _colCount;
};

/**
 * @class GridContainer grid.hpp "cinek/overview/grid.hpp"
 * @brief Defines a window within a Grid object.
 * Supports STL styles iteration of rows, where the value of each row is a 
 * pair of pointers marking the start and the end of a row.
 */
template<class GridType>
class GridContainer
{
public:
    /** 
     *  Defines a start and end point for T items in a row.  The end pointer
     *  points past the last item in the strip (iteration should stop if the compared
     *  pointer is greater than or equal to the end pointer.
     */ 
    typedef typename GridType::row_strip row_strip;
    /**
     *  Const version of row_strip.
     */
    typedef typename GridType::const_row_strip const_row_strip;

    GridContainer() :
        _grid(nullptr), _row(0), _column(0), _rowCount(0), _colCount(0) {}

    /** 
     * Initializes a container for a grid object.
     * @param grid The grid object conforming to the Grid<T> class specficiation.
     */
    GridContainer(GridType& grid) :
        _grid(&grid), _row(0), _column(0),
        _rowCount(grid.rowCount()), _colCount(grid.columnCount()) {}

    /**
     * Initializes a container for a portion of the specified grid object.
     * @param grid The grid object conforming to the Grid<T> class specficiation.
     * @param row       The upper bound of the specified grid.
     * @param column    The left bound of the specified grid.
     * @param rowCount  Number of rows to map from the specified grid.
     * @param colCount  Number of columns to map from the specified grid.
     */
    GridContainer(GridType& grid, 
            uint32_t row, 
            uint32_t column, 
            uint32_t rowCount, 
            uint32_t colCount
        ) :
        _grid(&grid), _row(row), _column(column),
        _rowCount(rowCount), _colCount(colCount) {}

    /**
     * Copy constructor.  Note that this is a shallow copy (the parent grid is not
     * copied.)
     * @param rhs The container to copy.
     */
    GridContainer(const GridContainer& rhs) :
        _grid(rhs._grid), _row(rhs._row), _column(rhs._column),
        _rowCount(rhs._rowCount), _colCount(rhs._colCount) {}
    
    /** @return The starting row index from the grid. */
    uint32_t rowOrigin() const { return _row; }
    /** @return The starting column index from the grid. */
    uint32_t colOrigin() const { return _column; }

    /** @return number of rows in the container. */
    uint32_t rowCount() const { return _rowCount; }
    /** @return number of columns in the container. */
    uint32_t colCount() const { return _colCount; }
    /**
     * Returns the row_strip at the specified row relative to the parent grid.
     * @param  row       The row within the container.
     * @return           The row_strip.
     */
    row_strip atRow(uint32_t row) { 
        return _grid ? _grid->atRow(_row+row, _column, _colCount) : row_strip();
    }
    /**
     * Returns the const_row_strip at the specified row relative to the parent grid.
     * @param  row       The row within the container.
     * @return           The const_row_strip.
     */
    const_row_strip atRow(uint32_t row) const {
        return _grid ? _grid->atRow(_row+row, _column, _colCount) : const_row_strip(); 
    }

    /**
     * std::iterator support
     */
    template<typename ValueType>
    class base_iterator : 
        public std::iterator<std::input_iterator_tag, ValueType>
    {
        GridType* _grid;
        uint32_t _row, _col;
        uint32_t _colcount;
    public:
        /** @cond */
        base_iterator() : _grid(nullptr), _row(0), _col(0), _colcount(0) {}
        base_iterator(GridType* x, uint32_t row, uint32_t col, uint32_t colcount) : 
            _grid(x), _row(row), _col(col), _colcount(colcount) {}
        base_iterator(const base_iterator& mit) : 
            _grid(mit._grid), _row(mit._row), _col(mit._col), _colcount(mit._colcount) {}
        base_iterator& operator++() {
            ++_row;
            return *this;
        }
        base_iterator operator++(int) {
            iterator tmp(*this); 
            operator++(); 
            return tmp;
        }
        bool operator==(const base_iterator& rhs) {
            return _grid == rhs._grid && _row == rhs._row && _col==rhs._col && 
                _colcount==rhs._colcount;
        }
        bool operator!=(const base_iterator& rhs) {
            return _grid != rhs._grid || _row != rhs._row || _col!=rhs._col || 
                _colcount!=rhs._colcount;
        }
        ValueType operator*() {
            return _grid->atRow(_row, _col, _colcount);
        }
        /** @endcond */
    };
    /** Basic input iterator */
    typedef base_iterator<typename GridType::row_strip> iterator;
    /** Basic input const iterator */
    typedef base_iterator<typename GridType::const_row_strip> const_iterator;

    /** @return An STL iterator pointing at the first row. */
    iterator begin() {
        return iterator(_grid, _row, _column, _colCount);
    }
    /** @return An STL const iterator pointing at the first row. */
    const_iterator begin() const {
        return const_iterator(_grid, _row, _column, _colCount);
    }
    /** @return An STL iterator pointing beyond the end row. */
    iterator end() {
        return iterator(_grid, _row + _rowCount, _column, _colCount);
    }
    /** @return An STL const iterator pointing beyond the end row. */
    const_iterator end() const {
        return const_iterator(_grid, _row + _rowCount, _column, _colCount);
    }

private:
    GridType* _grid;
    uint32_t _row, _column;
    uint32_t _rowCount, _colCount;
};


///////////////////////////////////////////////////////////////////////////////
template<typename Value>
Grid<Value>::Grid() :
    _data(nullptr),
    _rowCount(0),
    _colCount(0)
{
}

template<typename Value>
Grid<Value>::Grid(uint32_t rowCount, uint32_t columnCount) :
    _data(nullptr),
    _rowCount(rowCount),
    _colCount(columnCount)
{
    _data = (Value*)malloc(_rowCount * _colCount * sizeof(Value));
}

template<typename Value>
Grid<Value>::~Grid()
{
    if (_data != nullptr)
    {
        free(_data);
    }
}

template<typename Value>
Grid<Value>::Grid(Grid&& grid) noexcept :
    _data(grid._data),
    _rowCount(grid._rowCount),
    _colCount(grid._colCount)
{
    grid._data = nullptr;
    grid._rowCount = 0;
    grid._colCount = 0;
}

template<typename Value>
Grid<Value>& Grid<Value>::operator=(Grid&& grid) noexcept {
    _data = grid._data;
    _rowCount = grid._rowCount;
    _colCount = grid._colCount;

    grid._data = nullptr;
    grid._rowCount = grid._colCount = 0;

    return *this;
}
template<typename Value>
const Value& Grid<Value>::at(uint32_t row, uint32_t col) const
{
    const Value* rowData = _data + (_colCount * row);
    return *(rowData + col);
}

template<typename Value>
Value& Grid<Value>::at(uint32_t row, uint32_t col)
{
    return const_cast<Value&>(
            reinterpret_cast<const Grid<Value>* >(this)->at(row, col)
        );
}


template<typename Value>
typename Grid<Value>::const_row_strip 
    Grid<Value>::atRow(uint32_t row, uint32_t col, uint32_t length /*=0*/) const
{
    if (row >= _rowCount || col >= _colCount)
        return const_row_strip();
    const Value* rowStart = _data + (_colCount * row);
    if (length == UINT32_MAX)
        length = _colCount - col;
    uint32_t endCol = std::min(col+length, _colCount);
    return std::make_pair(rowStart+col, rowStart+endCol);
}

template<typename Value>
typename Grid<Value>::row_strip 
    Grid<Value>::atRow(uint32_t row, uint32_t col, uint32_t length /*=0*/)
{
    if (row >= _rowCount || col >= _colCount)
        return row_strip();
    Value* rowStart = _data + (_colCount * row);
    if (length == UINT32_MAX)
        length = _colCount - col;
    uint32_t endCol = std::min(col+length, _colCount);
    return std::make_pair(rowStart+col, rowStart+endCol);
}

    } /* overview */ 
} /* cinekine */

#endif