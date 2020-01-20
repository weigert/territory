/*
Group Class:
-> Contains a reference to member bots.
-> Can Contain References to Subgroups
-> Have a concept of landownership
-> Have a founding document which dictates large scale Behavior
-> Can have other tags that specify what they do
-> Act as a data structure to manipulate
-> Tasks can then manipulate the group structure
*/


enum grouprelationship{
  PARENT,
  CHILD,
  FRIEND,
  ENEMY
};


class Group{
public:
  //Parent and Children References
  Group* parent;
  std::vector<Group*> children;

  //Root and ID
  bool isRoot;
  int ID;

  //Membership
  std::vector<Bot*> members;

  //Other properties of the group...

  /*
  Compute Shared Group Ancestor!

  We need to be able to compute relations between bots within groups.

  Desired Properties:
    If a group is the parent group of another group, that is a specific type of relationship.
    If a group is a child of a group, that is a specific type of relationship.
    Otherwise groups either have a nearest common ancestor or they don't.
    This will dictate group behaviors towards each other.

  -> 0, no shared ancestor
  ->
  */

  /*
  Land Ownership?
  Founding Document?
  Mantras?
  Leadership?
  Internal Bot Hierarchy?
  Symbol?
  */

  /*
  Methods:
    -> Add Bot Member
    -> Remove Bot Member
    -> isMember of group??
    ->

  Bots should be able to test if bots are members of their group or any subgroups
  Bots should be able to expore the group tree and determine what they group relation is.

  Bots could have a similar relationship tree.
  With a single parent it is not possible to have loops.
  All you need to make sure is that a group strictly has a parent, or is considered a root node.

  */
};
