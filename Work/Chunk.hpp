/*
© 2019, Dark Orb.

The license version - 1.0

This license is hereby grants to any person who obtained a copy of this software the next rights to:
1. Use and do reverse-engineering of compiled version of this software at no cost, without any restrictions, in non-commercial and commercial purposes
2. Use source codes of this software at no cost but with the limitations - source codes available only for non-commercial, academic and / or scientific purposes
3. Copy and distribute without any fee
4. Create a copy of the original repository and / or create own derivative software for non-commercial,  academic and / or scientific purposes only

This license is require to:
1. Keep the full license text without any changes
2. The license text must be included once in a file called 'License' which placed in the root directory of the software and in all source files of the software

This license is deny to:
1. Change license of the derivative software
2. Use the copyright holder name and name of any contributor of this software for advertising derivative software without legally certified permission
3. Sell this software without an author legally certified permission

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <string>

using std::string;
using std::wstring;

#include <vector>

using std::vector;

#include <map>
#include <set>

using std::map;
using std::set;
using std::pair;

namespace Chunk {
  const static size_t cChunkSize = 100;
  const static size_t cChunkOffsetSize = 100;

  typedef enum {
    D_IN,
    D_OUT
  } enDirection;

  template<typename T>
  class CRaw {
    private:
    static size_t fToIn( const T *_Value, const size_t _Size, const T _Target,
                         const size_t _PieceSize, const size_t _OffsetSize ) {
      if( _Size <= 1 )
        return 0;

      long vLeftStartPosition = ( _Size / 2 ) - _PieceSize,
        vLeftEndPosition = ( _Size / 2 );

      long vRightStartPosition = ( _Size / 2 ) + _PieceSize + 1,
        vRightEndPosition = ( _Size / 2 ) + 1;

      if( vLeftStartPosition < 0 )
        vLeftStartPosition = 0;

      if( vRightStartPosition >= _Size )
        vRightStartPosition = _Size - 1;

      size_t c = 0;
      while( c < _Size ) {
        for( long b = vLeftStartPosition; b <= vLeftEndPosition; b++, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        for( long b = vRightStartPosition; b >= vRightEndPosition; b--, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        vLeftEndPosition = vLeftStartPosition;
        vLeftStartPosition = vLeftStartPosition - _OffsetSize < 0 ? 0 : vLeftStartPosition - _OffsetSize;

        vRightEndPosition = vRightStartPosition;
        vRightStartPosition = vRightStartPosition + _OffsetSize + 1 >= _Size ? _Size - 1 : vRightStartPosition + _OffsetSize + 1;
      }

      return 0;
    }

    static size_t fToOut( const T *_Value, const size_t _Size, const T _Target,
                          const size_t _PieceSize, const size_t _OffsetSize ) {
      if( _Size <= 1 )
        return 0;

      long vLeftStartPosition = 0, vLeftEndPosition = _PieceSize;

      long vRightStartPosition = _Size - 1,
        vRightEndPosition = ( _Size - 1 ) - _PieceSize;

      if( vRightEndPosition <= ( _Size / 2 ) + 1 || ( _Size / 2 ) + vRightEndPosition > _Size )
        vRightEndPosition = ( _Size / 2 ) + 1;

      size_t c = 0;
      while( c < _Size ) {
        for( long b = vLeftStartPosition; b <= vLeftEndPosition; b++, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        for( size_t b = vRightStartPosition; b >= vRightEndPosition; b--, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        vLeftStartPosition = vLeftEndPosition;
        vLeftEndPosition = vLeftEndPosition + _OffsetSize > _Size / 2 ? _Size / 2 : vLeftEndPosition + _OffsetSize;

        vRightStartPosition = vRightEndPosition;
        vRightEndPosition = vRightEndPosition - _OffsetSize <= ( _Size / 2 ) + 1 ? ( _Size / 2 ) + 1 : vRightEndPosition - _OffsetSize;
      }

      return 0;
    }

    public:
    static size_t fGet( const T *_Value, const size_t _Size, const T _Target,
                        const enDirection _Direction,
                        const long _PieceSize = -1, const long _OffsetSize = -1 ) {
      switch( _Direction ) {
        case D_IN:
        if( _PieceSize >= 0 ) {
          if( _OffsetSize >= 0 )
            return CRaw<T>::fToIn( _Value, _Size, _Target, _PieceSize, _OffsetSize );
          else
            return CRaw<T>::fToIn( _Value, _Size, _Target, _PieceSize, cChunkOffsetSize );
        } else {
          if( _OffsetSize >= 0 )
            return CRaw<T>::fToIn( _Value, _Size, _Target, cChunkSize, _OffsetSize );
          else
            return CRaw<T>::fToIn( _Value, _Size, _Target, cChunkSize, cChunkOffsetSize );
        }
        break;

        case D_OUT:
        if( _PieceSize >= 0 ) {
          if( _OffsetSize >= 0 )
            return CRaw<T>::fToOut( _Value, _Size, _Target, _PieceSize, _OffsetSize );
          else
            return CRaw<T>::fToOut( _Value, _Size, _Target, _PieceSize, cChunkOffsetSize );
        } else {
          if( _OffsetSize != 0 )
            return CRaw<T>::fToOut( _Value, _Size, _Target, cChunkSize, cChunkOffsetSize );
          else
            return CRaw<T>::fToOut( _Value, _Size, _Target, cChunkSize, cChunkOffsetSize );
        }
        break;
      }

      return 0;
    }
  };

  class CContainer {
    public:
    static inline size_t fGet( const string _Value, const char _Target, const enDirection _Type,
                               const size_t _PieceSize, const size_t _OffsetSize ) {
      return CRaw<char>::fGet( _Value.c_str(), _Value.length(),
                               _Target, _Type,
                               _PieceSize, _OffsetSize );
    }

    template<typename T>
    static size_t fGet( const vector<T> _Collection, const T _Target, const enDirection _Type,
                        const size_t _PieceSize, const size_t _OffsetSize ) {
      return CRaw<T>::fGet( _Collection.data(), _Collection.size(),
                            _Target, _Type,
                            _PieceSize, _OffsetSize );
    }

    template<typename T1, typename T2>
    static size_t fGet( const map<T1, T2> _Collection, const T1 _Target, const enDirection _Type,
                        const size_t _PieceSize, const size_t _OffsetSize ) {
      vector<T1> vGetKeys;

      for( const pair<T1, T2> &vPair : _Collection )
        vGetKeys.push_back( vPair.first );

      return CRaw<T1>::fGet( vGetKeys.data(), vGetKeys.size(),
                             _Target, _Type,
                             _PieceSize, _OffsetSize );
    }

    template<typename T1, typename T2>
    static size_t fGet( const map<T1, T2> _Collection, const T2 _Target, const enDirection _Type,
                        const size_t _PieceSize, const size_t _OffsetSize ) {
      vector<T2> vGetValues;

      for( const pair<T1, T2> &vPair : _Collection )
        vGetValues.push_back( vPair.second );

      return CRaw<T2>::fGet( vGetValues.data(), vGetValues.size(),
                             _Target, _Type,
                             _PieceSize, _OffsetSize );
    }

    template<typename T>
    static size_t fGet( const set<T> _Collection, const T _Target, const enDirection _Type,
                        const size_t _PieceSize, const size_t _OffsetSize ) {
      vector<T> vGetValues;

      for( const T &vItem : _Collection )
        vGetValues.push_back( vItem );

      return CRaw<T>::fGet( vGetValues.data(), vGetValues.size(),
                            _Target, _Type,
                            _PieceSize, _OffsetSize );
    }
  };
}
