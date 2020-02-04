/*
© 2019, Serge Page.

This license is hereby grants to any person who obtained a copy of this product or the product source files the next rights to:

- Use a compiled version of this product at no cost, without any restrictions, in non-commercial and commercial purposes
- Do reverse-engineering of this product in non-commercial purposes only
- Use source codes of this product at no cost but with the limitations - source codes available only for non-commercial, academic and / or scientific purposes
- Copy and distribute without any fee
- Copy of the original repository and / or create own derivative product for non-commercial,  academic and / or scientific purposes only
- Link the product source code with an another product source code which licensed under any of Dark Orb licenses or one of these licenses:
  - MIT License
  - Microsoft Public License
  - Beerware License
  - Academic Free License
  - WTFPL
  - Unlicense
  - Original BSD license
  - Modified BSD License
  - Simplified BSD License
  - Zero Clause BSD
- Link the product source code with an another product source code if between them no any patent collision

This license is require to:

- Keep the full license text without any changes
- The license text must be included once in a file called 'License' which placed in the root directory of the product and in all source files of the product

This license is deny to:

- Change license of the derivative product
- Use the product’s author name and name of any contributor of this product for advertising derivative software without legally certified permission
- Resell this product
- Use the product or the product source code for any purpose which refers to any government of any country

The product is an original source codes and original compiled files which made by the original author and provided only under the grants and restrictions of this license. All damages which can be happen after and while using the product will not be compensate.
*/

#pragma once

#include <string>

using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

#include <vector>

using std::vector;

#include <map>
#include <set>

using std::map;
using std::set;
using std::pair;

namespace Chunk {
  const static uint32_t СChunkSize = 100;
  const static uint32_t СChunkOffsetSize = 100;

  enum class ENDirection : int64_t {
    D_IN,
    D_OUT
  };

  template<typename T>
  class CRaw {
    private:
    static uint32_t FToIn( const T *_Value, const uint32_t _Size, const T _Target,
                         const uint32_t _PieceSize, const uint32_t _OffsetSize ) {
      if( _Size <= 1 )
        return 0;

      long VLeftStartPosition = ( _Size / 2 ) - _PieceSize,
        VLeftEndPosition = ( _Size / 2 );

      long VRightStartPosition = ( _Size / 2 ) + _PieceSize + 1,
        VRightEndPosition = ( _Size / 2 ) + 1;

      if( VLeftStartPosition < 0 )
        VLeftStartPosition = 0;

      if( VRightStartPosition >= _Size )
        VRightStartPosition = _Size - 1;

      uint32_t c = 0;
      while( c < _Size ) {
        for( long b = VLeftStartPosition; b <= VLeftEndPosition; b++, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        for( long b = VRightStartPosition; b >= VRightEndPosition; b--, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        VLeftEndPosition = VLeftStartPosition;
        VLeftStartPosition = VLeftStartPosition - _OffsetSize < 0 ? 0 : VLeftStartPosition - _OffsetSize;

        VRightEndPosition = VRightStartPosition;
        VRightStartPosition = VRightStartPosition + _OffsetSize + 1 >= _Size ? _Size - 1 : VRightStartPosition + _OffsetSize + 1;
      }

      return 0;
    }

    static uint32_t FToOut( const T *_Value, const uint32_t _Size, const T _Target,
                          const uint32_t _PieceSize, const uint32_t _OffsetSize ) {
      if( _Size <= 1 )
        return 0;

      long VLeftStartPosition = 0, VLeftEndPosition = _PieceSize;

      long VRightStartPosition = _Size - 1,
        VRightEndPosition = ( _Size - 1 ) - _PieceSize;

      if( VRightEndPosition <= ( _Size / 2 ) + 1 || ( _Size / 2 ) + VRightEndPosition > _Size )
        VRightEndPosition = ( _Size / 2 ) + 1;

      uint32_t c = 0;
      while( c < _Size ) {
        for( long b = VLeftStartPosition; b <= VLeftEndPosition; b++, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        for( uint32_t b = VRightStartPosition; b >= VRightEndPosition; b--, c++ ) {
          if( _Value [ b ] == _Target )
            return b;
        }

        VLeftStartPosition = VLeftEndPosition;
        VLeftEndPosition = VLeftEndPosition + _OffsetSize > _Size / 2 ? _Size / 2 : VLeftEndPosition + _OffsetSize;

        VRightStartPosition = VRightEndPosition;
        VRightEndPosition = VRightEndPosition - _OffsetSize <= ( _Size / 2 ) + 1 ? ( _Size / 2 ) + 1 : VRightEndPosition - _OffsetSize;
      }

      return 0;
    }

    public:
    static uint32_t FGet( const T *_Value, const uint32_t _Size, const T _Target,
                        const ENDirection _Direction,
                        const long _PieceSize = -1, const long _OffsetSize = -1 ) {
      switch( _Direction ) {
        case D_IN:
        if( _PieceSize >= 0 ) {
          if( _OffsetSize >= 0 )
            return CRaw<T>::FToIn( _Value, _Size, _Target, _PieceSize, _OffsetSize );
          else
            return CRaw<T>::FToIn( _Value, _Size, _Target, _PieceSize, СChunkOffsetSize );
        } else {
          if( _OffsetSize >= 0 )
            return CRaw<T>::FToIn( _Value, _Size, _Target, СChunkSize, _OffsetSize );
          else
            return CRaw<T>::FToIn( _Value, _Size, _Target, СChunkSize, СChunkOffsetSize );
        }
        break;

        case D_OUT:
        if( _PieceSize >= 0 ) {
          if( _OffsetSize >= 0 )
            return CRaw<T>::FToOut( _Value, _Size, _Target, _PieceSize, _OffsetSize );
          else
            return CRaw<T>::FToOut( _Value, _Size, _Target, _PieceSize, СChunkOffsetSize );
        } else {
          if( _OffsetSize != 0 )
            return CRaw<T>::FToOut( _Value, _Size, _Target, СChunkSize, СChunkOffsetSize );
          else
            return CRaw<T>::FToOut( _Value, _Size, _Target, СChunkSize, СChunkOffsetSize );
        }
        break;
      }

      return 0;
    }
  };

  class CContainer {
    public:
    template<typename Source, typename SourceChar>
    static inline uint32_t FGet( const Source _Value, const char _Target, const ENDirection _Type,
                               const uint32_t _PieceSize, const uint32_t _OffsetSize ) {
      return CRaw<SourceChar>::FGet( _Value.c_str(), _Value.length(),
                               _Target, _Type,
                               _PieceSize, _OffsetSize );
    }

    template<typename T>
    static uint32_t FGet( const Vector<T> _Collection, const T _Target, const ENDirection _Type,
                        const uint32_t _PieceSize, const uint32_t _OffsetSize ) {
      return CRaw<T>::FGet( _Collection.data(), _Collection.size(),
                            _Target, _Type,
                            _PieceSize, _OffsetSize );
    }

    template<typename T1, typename T2>
    static uint32_t FGet( const map<T1, T2> _Collection, const T1 _Target, const ENDirection _Type,
                        const uint32_t _PieceSize, const uint32_t _OffsetSize ) {
      Vector<T1> VGetKeys;

      for( const pair<T1, T2> &VPair : _Collection )
        VGetKeys.push_back( VPair.first );

      return CRaw<T1>::FGet( VGetKeys.data(), VGetKeys.size(),
                             _Target, _Type,
                             _PieceSize, _OffsetSize );
    }

    template<typename T1, typename T2>
    static uint32_t FGet( const map<T1, T2> _Collection, const T2 _Target, const ENDirection _Type,
                        const uint32_t _PieceSize, const uint32_t _OffsetSize ) {
      Vector<T2> VGetValues;

      for( const pair<T1, T2> &VPair : _Collection )
        VGetValues.push_back( VPair.second );

      return CRaw<T2>::FGet( VGetValues.data(), VGetValues.size(),
                             _Target, _Type,
                             _PieceSize, _OffsetSize );
    }

    template<typename T>
    static uint32_t FGet( const set<T> _Collection, const T _Target, const ENDirection _Type,
                        const uint32_t _PieceSize, const uint32_t _OffsetSize ) {
      Vector<T> VGetValues;

      for( const T &VItem : _Collection )
        VGetValues.push_back( VItem );

      return CRaw<T>::FGet( VGetValues.data(), VGetValues.size(),
                            _Target, _Type,
                            _PieceSize, _OffsetSize );
    }
  };
}
