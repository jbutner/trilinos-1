#ifndef _FORBLOCKRTC_H
#define _FORBLOCKRTC_H

#include "LineRTC.hh"
#include "BlockRTC.hh"
#include "TokenizerRTC.hh"

#include <string>
#include <map>

namespace PG_RuntimeCompiler {

/** 
 * A ForBlock represents a block of code within a for-loop. 
 * ForBlock extends Block because it is a Block.
 */

class ForBlock : public Block
{
 public:

  /**
   * Constructor -> The constructor contructs the parent Block, finds and 
   *                creates each component of the for loop, and tells parent to
   *                create its sub statements. 
   *
   * @param vars  - A map of already active variables
   * @param lines - The array of strings that represent the lines of the code. 
   * @param errs  - A string containing the errors that have been generated by
   *                the compiling of lines. If errs is not empty, then the 
   *                program has not compiled succesfully
   */
  ForBlock(std::map<std::string, Variable*> vars, Tokenizer& lines, 
           std::string& errs);

  /**
   ***************** Destructor *******************
   * The destructor deletes _condition, _init, and _postloop.
   */
  ~ForBlock();

  /*
   * execute -> This method executes this ForBlock. The _init is run, 
   *            _condition is evaluated and the loop enters if _condition is 
   *            true. After each loop, _postloop is run and _condition is 
   *            re-evaluated to determine if we loop again.
   */
  Value* execute();

 private:

  Line* _init; /**!< The line of code that is run when the for-loop is entered.
                *    For example, for:  for(int i = 0; i < 10; ++i)...
                *    int i = 0 is the _init
                */

  Line* _condition; /**!< The line thats checked to see if the loop should stop
                     *    For example, for:  for(int i = 0; i < 10; ++i)...
                     *    i < 10 is the _condition
                     */

  Line* _postloop;  /**!< The line that is run at the end of every loop. 
                     *    For example, for:  for(int i = 0; i < 10; ++i)...
                     *    ++i is the _postloop
                     */
};

}

#endif
