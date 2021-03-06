#ifndef TTABLE_H
#define TTABLE_H

#include "../utils/types.h"

#include <vector>
#include <utility>
#include <string>

namespace asearch
{

typedef U32 Move; ///< Actual move (index to a move table)
typedef I16 Score; ///< Minimax value in {-32768, 32768}
typedef U64 HashType; ///< 64 bit unsigned int hash type

/**
 * @abstract TTable
 *
 * @brief
 * ASearch specific transposition table based on Zobrist scheme.
 */
class TTable
{
public:
  enum Flag
  {
    INVALID, ///< Entry doesn't exist
    OUTDATED, ///< Entry's depth < current depth and not a mate score
    UPPERBOUND, ///< Entry's score <= alpha
    LOWERBOUND, ///< Entry's score >= beta
    QUIESCENT, ///< Quiescent() score i.e. depth == 0
    EXACT ///< Exact score i.e. leaf node in full minimax tree
  };

  /// Constructor
  TTable() {}

  /// Virtual destructor
  virtual ~TTable();

  /// Represent state as string
  virtual std::string ToString() = 0;

  /// Create hashkey from current state
  virtual void CreateHash() = 0;

  /// Determine wether state is winning or losing
  virtual bool IsMateScore(Score inScore) = 0;

  /// Initialize the hashtable and hashcodes. Ideally this should go in L3 cache
  static void Initialize(const U32 mbSize, const U32 numHashCodes);

  /// Set hashtable memory to 0, but don't free it
  static void Reset();

  /// Return hashtype as binary string
  static std::string Hash2String(HashType inHashType);

  /// Add an entry into the hash table
  void Put(U8 inDepth, U8 inPly, Score inAlpha, Score inBeta, Move inMove, Score inScore);

  /// Retrieve a pair from the hash table and return the flag type
  Flag Get(U8 inDepth, U8 inPly, Move &outMove, Score &outScore);

protected:
  static U32 sHits; ///< Cache hits or Get() calls
  static U32 sSuccessfullHits; ///< Cache hits with flag != INVALID
  static U32 sCollisions; ///< Hashtable collisions
  static U32 sHashSize; ///< Actual table size
  static HashType sHashMask; ///< Hash mask for retracting table entries
  static std::vector<HashType> sHashCodes; ///< Random bitstrings used for hashing

  HashType mHashKey; ///< Actual hashkey of a state

private:
  struct Entry
  {
    HashType mKey;       ///< Hashkey of this entry
    Score mValue;        ///< Best corresponding value
    U8 mDepth      : 5;  ///< Depth that the entry was calculated at
    U8 mFlag       : 3;  ///< Entry flag type see Flags
    Move mMove     : 22; ///< Encoded move
  } __attribute__((packed));

  static std::vector<Entry> sHashTable; ///< Hashtable containing entries
};

} // namespace asearch

#endif // TTABLE_H
