// Wish consists of constraints that define archetypes. An archetype
// is very similar to a class. In Wish, however, archetypes are defined
// by constraints. The members of an archetype can have any valid value
// for that type which conforms to the constraint. Because of this,
// archetypes are incomplete; they must be instantiated to become
// objects and the constraints put on the archetype define the possible
// values of that object.

// When instantiated, an object's members are set to any random value
// which meets the constraints of the archetype used; individual objects
// may be instantiated with further constraints so long as the constraint
// given is at most as liberal as those given in the archetype definition.

// This is an empty archetype. A lone identifier is assumed to be a
// definition of a new archetype.
Foo;
// This defines a functionally identical archetype; it has no members
// and no constraints.
Bar | {};
/*
<such-that> -> `:`
<constraint-terminator> -> `;`
<open-brace> -> `{`
<close-brace> -> `}`
<comma> -> `,`
<identifier> -> `[azAZ_][azAZ_09]*`
<archetype> -> <identifier><such-that><constraint-list><constraint-terminator>
<constraint-list> -> <constraint-scope>(<comma><constraint-scope>)*
*/

// To create a new instance of Foo we use the 'instance' keyword.
foo1 | instance Foo;
// Which can be shortened using the '!>' operator
foo2 !! Foo;
// Instances are cleaned up when they go out of scope.

// If you want to refer to but not own an instance, use a reference.
// References must be initialized from another value.
foo3 reference | Foo; // foo3 is not initialized; only only allowed in declarations
foo4 reference !! foo1; // foo4 is initialized by foo1 and adopts it's archetype
// Which also has a shorthand version that uses '@!':
foo3short @ Foo;
foo4short @! foo2;  // An instance of a reference must be initialized from another
                    // instance.

// Lastly, nothing in Wish is nullable, but there is a construct that
// simulates it in a way that forces you to handle it. It's called a
// conjunctive archetype, though the keyword is 'optional'. It shares
// functionality with C/C++ unions and is a version of the monad
// abstraction.
//
// All conjunctive archetypes have at least two possible archetypes.
// One of the archetypes is always the archetype 'nothing'.
// If no implementation for 'nothing' is provided, then one is
// generated using an empty archetype '{}'.
foo5 optional | Foo, Bar;
foo6 optional !! foo2;
// Its shorthand uses '?!':
foo5short ? Foo;
foo6short ?! foo2;
// This actually means that foo5 can have at least two different values
// that inherit different archetypes. An empty archetype is added to the
// end of the list of possible archetypes, accessible by the keyword
// 'nothing'.

// When accessing 'foo5' or 'foo6', you must use the '??' conjunctive
// enumeration operator, here shown inside a function definition:
inspectFoo( foo ? Foo ) | truth { foo ?? { true; }, {false;}; };
// The '??' operator cycles through the ordered list of possible
// archetypes for the instance being tested, and executes the code
// associated with the archetype the instance actually uses. The last
// code block always corresponds to the empty archetype. This can be
// implemented in actual machine code using a local jump table combined
// with table offsets stored in the conjunctive archetype.

// You can assign identifiers to the different archetypes and
// use '?' to match by name instead using the '->' operator; the
// empty archetype uses the keyword 'nothing':
Jif ? `Something`->{Foo}; // Not sold on backticks here?
inspectJif( jif ? Jif ) | truth { jif ?? nothing->{false;}, Something->{true;}; };

// Wish has several built-in archetypes, which are analogous 
// to the POD of C-like langauges.

// The logical type is called "truth" and can have the value of either
// of the two logical literals 'true' and 'false'.
truthiness !! truth {true, false};

// Symbols are similar to enum values or the same construct in ECMAscript.
// Symbols are not convertible to integers, unlike in C/C++. They can
// belong to a collection, however.
id1 !! symbol {'someSymbol'};

// A default integer is a 64bit signed number.
integralValue1 !! integer { core::integers::min, core::integers::max };
// Here the initial value is 4, but we can change that later because
// instances don't permanently constrain the archetype they inherit.
integerValue2 !! integer{ 4 };

// Unsigned integers are denoted by the 'positive' constraint.
integralValue3 !! integer{ positive({0, core::integers::positive::max}); };

// An integer that only consists of two bytes.
integerValue3 !! integer{bytes(2)};
// An integer that consists only of two bits.
integerValue4 !! integer{bits(2)};

// Note: The '?' shouldn't be infectious, at least not on function definitions.
memory<type Object> ? 
something->{
    visible {
        `instance`( obj @ Object )
        | memory<Object>
        {
            value << obj;
            return instance;
        };
        `$`() @ Object { return value; };
        // Hmmmmm
        `#`() ? core::system::address { return addressof(value); };
    },
    hidden {
        value | Object;
    };
}, nothing->{
    `instance`() | memory<Object> { return instance; };
};

// ======== Memory ========
//
// The 'done thing' with new languages is usually to define a memory
// model that is less hands-on than C/C++. Rust doesn't seem to provide
// addresses directly (or else it is somewhat complicated) and relies on
// scopes and lifetimes. Java, C#, JavaScript, and Python use garbage
// collection. Can we come up with something better?
//
// If we provide references, unique<> and a shared<> conjunctive archetype,
// this handles many of not most situations. However, the implementation of
// shared<> basically requires some way to keep the object alive.
// Should references just...do that? How is that different from shared<>?
// I don't think it is. An instance, therefore, stays alive so long
// as the scope does OR until there are no references to it left.
// 
// So how does unique<> work? If we use the '<<' operator to mean 'move'
// close to the C++ sense, then it can be implemented.

// In wish, the assignment operator ':=' always copies the value of
// one instance into another. This is because instances are cleaned up
// when they go out of scope, so assignment can't take any chances.
// The move assignment operator, '<<', is similar. By default, it
// copies the value of the right hand side into the left hand side
// unless both operands are variadic in which case it moves the
// object and mutates the right hand side to nothing. If there is
// an operator overload for the operands, then that is used instead.
// If you want to transfer ownership, then use the 'unique' built-in:

unique<type Object> ? 
something->{ visible {
    `:=`( other @ unique<Object>) @ unique<Object>
    {
        other ?? {
            obj << other.obj;
        },{
            instance->nothing;
        }
    };
    `$`() | @Object { return obj; };
},
hidden {
    obj | Object;
}},
nothing->{
    `:=`( other @ unique<Object>) @ unique<Object>
    {
        other ?? {
            instance->something;
            obj << other.obj;
            return instance;
        },{
            return instance;
        }
    };
    `$`() | nothing;
};

unq ?! unique<Cuz>;

// Maybe just call it unique? Does this even make sense in our use case?
// We haven't determined how dynamic binding even works, and surely the
// Java solution of making everything virtual is foolish. The real question is,
// what kind of ID can have it's type resolved at runtime?

// There is a way to make manipulate memory addresses: use the memory archetype
// and indicate the archetype you want to access. Note that memory is
// a variadic archetype, so a value that inherits memory must also be
// variadic.
ptr ?! memory<Foo>;

// The memory archetype is special; to get a reference to the actual
// object, you need to use the '$' operator (for "$ubstitution").
// Note: Here we want to combine the '?' and the '@' tokens. I think
// this is valid because here '?' is more of an declarative operator
// and '@' has to do with types.
inspectFooAddress( foo ? memory<Foo> ) ? @Foo { foo ?? {$foo;}, {}; };

// Because the referenced type is known, you can assign to memory<> directly:
a !! Foo;
b ?! memory<Foo>{a};
c ?! memory<Foo>;
// 'c' references the memory location of a.
c := a;

// Promises should be built in. Not sure about all the details; note
// the '&' character is free, and it looks like a pretty good candidate
// for the "then" operator. Naturally promises are conjunctive?

`promise`<type Object> ? `something`->{ obj | Object }, `error`->{ err | reason };

PromisedFoo ? promise<Foo>;
getPromise() | PromisedFoo { return  }


/*
<kw-instance> -> `instance`
<instantiating-expression> -> <identifier> <such-that> <kw-instance> <identifier> <constraint-terminator>
*/

// An archetype may inherit from another archetype.
Foo | {};
Bar | Foo, {};
Sop | {};
// An instance may only use a single archetype.
// oof : instance Foo, Bar; <- Not allowed!
// But an archetype may inherit any number of other archetypes.
Baz | Foo, Bar, {};
oof | instance Baz, {}; // Much better.

// An archetype may contain any number of members.
Zat | {
    foo | Foo;
    bar | Bar;
};

// Archetypes have access modifiers which control which code can see what.
Bif |
visible {
    foo | Foo; // Outside code can see member 'foo'
},
internal {
    sop | Sop; // Any archetype that inherits from this one can see member 'sop'
},
hidden {
    bar | Bar; // Outside code can not see member 'bar'
},
invite {
    // Anything declared in here is treated as a reference to an indentifier
    // external to this archetype which has access to 'internal' and 'hidden'
    // members.
};

// Note that access rules naturally extend to initializing expressions;
// that is, they cannot modify hidden members.

// Members of an archetype can be constrained themselves
Qud | {
    count | integer {0,100}; // Any whole number from 0 to 100
};
// Instances of such an archetype have any value that conforms to the constraint.
qud1 !! Qud; // 'count' could be any whole number from 0 to 100
// And instantiations can add further constraints on initialization so long as
// they are within the existing constraints.
qud2 !! Qud { count | {5,50}; }; // Now any whole number from 5 to 50
// qud3 | instance Qud { count | {-3,50}; }; // Not allowed!

// Any expression can denote a constraint, including executable code.
Rak | {
    property | integer { return 4+5; };
};
// Assigning to a callable member is impossible because there is no way
// to guarantee that the constraint placed on the return value by the
// callable body encloses the new constraint.
rak | instance Rak;
// rak.property = -2; // Not allowed! No way to guarantee constraints valid

// Callable members can have arguments, naturally, and those arguments
// can be constrained.
Yok | {
    addFive( a | integer ) | integer { return a+5; };
    addEight( b | integer {1,6} ) | integer { return a+8; };
};

// It is safe to call a callable member without an instance
// if the compiler can tell that the executable code is not
// dependent on another member (CLion's presentation parser,
// for instance, can already tell this for C++).
answer1 | Yok::addFive(4);  // == 9
answer2 | Yok::addEight(4); // == 12
// answer3 : Yok.addEight(0); // Not allowed!

// You can indicate a constraint is for debugging only with the
// 'expect' keyword. The constraint is then only included in
// debug mode.
Gib | {
    subTwo( a | integer expect{>=2} ) | integer { return a-2; };
};

// Nothing in Wish is nullable, but there is a way to indicate
// that something might not exist and enforce handling it.
// Use option instead of such-that:
Taf |
hidden {
    foo ? Foo; // 'foo' may or may not exist
},
visible {
    getFoo ? Foo {
        // Use the optional control syntax to access
        // optional members. There is no way to access
        // 'foo' otherwise.
        foo ?? { return foo; },{ return nothing; };
    };
    setFoo( foo : @Foo ) : {
        // You can override the control syntax somewhat if
        // the consequences of the instance not existing are
        // not danagerous.
        instance.foo ! { instance.foo := foo; };
    };
};

// Operators can be overloaded for an archetype, using the backtick
// syntax. Backticks force the enclosed code to be read as an identifier
// instead of code. Backticks are not used anywhere else,
// and they've been chosen to be a little awkward on purpose to discourage
// operator overloading.
// Note: This is a WIP syntax, though it kinda makes sense.
Kuf :
visible {
    `==`( other : @constant Kuf ) constant : truth { value == other.value; };
    `><`( other : @constant Kuf ) constant : truth { value >< other.value; };
    // Is this how we should reference this object, with the 'instance' keyword?
    `:=`( other : @constant Kuf ) : @Kuf { value == other.value * 2; return instance; };
    // Hmmm, is this right? It kind of gets across the same idea as
    // a move reference but is quite explicit.
    `+`( other : @constant Kuf ) : Kuf { : Kuf { value : value + other.value }; };
    // Kind of a silly example, but it is just an illustration.
    `not`() : truth { if( value >= 0 ) { return false }; return true; };
    `?`(...{``:{};}) : hmmmmm; // Only monadic archetypes have this operator to overload;
                         // the parameter is a collection of code? I don't know how to
                         // express this.
},
hidden {
    value : integer;
},
// Like the "friend" keyword, except it is a scope.
invite {
    // Because we are in an 'invite' scope, we assume that the names
    // thus declared must exist outside this archetype.
    `<=`( lhs : @constant Kuf, rhs : @constant Kuf ) : truth;
};

// You can of course create free functions that overload operators.
`<=`( lhs : @constant Kuf, rhs : @constant Kuf )
    : truth
{ lhs.value <= rhs.value; };



// Here an archetype named "Item" is defined. It has three members:
// a string called 'name' that can contain any characters,
// 'weight', a number between 0.1 and 100.0, and 'baseValue', another
// number between 0 and 100. Note that baseValue omits the 'number'
// keyword because the constraint given makes it clear that the type
// of 'baseValue' is 'number'.
Item : {
    name : string;
    weight : number {0.1,100.0};
    baseValue : {0.0, 100.0};
};

// Rational values are ALWAYS optional
rationalValue1 ? instance rational{ num : integer; den : integer{<0,>0}; };
// Wait, should the '?' operator work differently? Should we allow custom
// monads?
Monad<type ValueObject> ?
`something`{{ value : ValueObject; }}, // Backticks or single quotes?
{{ reason : Error; }}; // The last case is always the default

intMonad1 : instance Monad<integer>{5};
intMonad2 : instance Monad<integer>;
exists( val : @Monad<integer> )
: truth
{ val ? something{ true; }, { false; }; };
exists(intMonad1); // == true
exists(intMonad2); // == false

// There is a complete set of operators for numbers, integers, and truth values
// + : addition
// - : subtraction, numerical negation
// / : division
// * : multiplication
// ~ : round number to nearest integer
// ~up, ~down : ceiling of number, floor of number
// % : round number to rational
// ++ : increment
// -- : decrement
// ** : multiply by self
// ^ : exponentiation
// |a| : absolute value of a
// |a|{ c,b } : clip a to range of values {c,b}
// mod : modulo
// not : logical negation
// and : logical and
// nand : logical not-and
// or : logical or
// nor : logical not-or
// xor : logical exclusive or
// ==, <. <=, >,>=,>< : equality, less-than, less-than-or-equal,
//                      greater-than, greater-than-or-equal, not-equal
// log : logarithm, base 10
// ln  : natural logarithm
// ^up, ^down : bit shifts
// a root b : bth root of a
// 0not,0and,0or,0nand,0nor,0xor : Bitwise boolean operations

// The number archetype corresponds to IEEE 64bit floating
// point numbers, otherwiser known as "double" in C/C++.
floatingPointValue : instance number{ core::numbers::min, core::numbers::max, };

// Strings are a built-in type. They are geared towards display
// and localization, so don't assume too many things about how
// they are structured.
text : instance string;

// Quests are a kind of asynchronous event handler.
main : instance quest;

// Here, the ellipsis operator indicates an unordered, unbounded
// collection of the archetype indicated. Collections always use
// commas to seperate the values.
// Note tha once constrained, Slot cannot have symbols added to
// it, and in this way it is similar to an 'enum' or array. In
// short, the size of Slot is static and the compiler can optimize
// based on that knowledge.
Slot : constant symbol... {
    'torso',
    'legs',
    'hands',
    'head',
    'feet'
};

// An empty archetype
Node:{};

// Because we haven't constrained the values that can exist in a
// List, this collection is unbounded and can be modified at
// run time. The compiler may choose to implement this collection
// as a linked list.
List : Node..., {};
list : instance List;

// A simple stack implementation which uses the option operator '?';
// This operator indicates that the object it modifies may or may not
// actually exist. It wraps the values with a monad, and no code may
// access the object underneath directly. Instead you must use the
// optional object handling interface. It superficially resembles
// the ternary operator of C/C++.
// We have also introduced a type constraint. The angle brackets indicate
// that the identifier or indentifiers within are placeholders for an
// archetype of archetypes to be bound when an instance of StackNode
// is created.
StackNode : <Object>, {
    next ? StackNode<Object>;
    object : Object;
}

Stack : <Object>,
// The 'visible' keyword denotes that the following constraints are
// accessible from outside teh archetype.
visible {
    empty : truth { head ? {false;},{true;}; };
    push( object : Object ) : {
        // The language can tell the difference between the two "object" identifiers
        // because only a member can appear in the lhs of a constraint.
        newHead : instance StackNode<Object> { next : head; object : object; };
        // It is immaterial if head exists or not, this code is valid,
        // but we still need to assure the language it is okay even if it
        // can prove it is okay itself.
        // The '!' operator is only used for this purpose; the 'not' keyword
        // is used for boolean negation instead.
        // You'll get a compiler warning, however, if the code looks dangerous
        // in the case that head is nothing
        head ! { head := newHead; };
    };
    peek ? Object {
         head ? { head.object;},{nothing;};
    };
    pop ? Object {
    // The option operator used to access an optional member.
        head ? {
            // If head exists, then execute this code.
            // Without the instance keyword, 'out' adopts the known
            // archetype of head.object.
            // Note that no override is needed here because this scope
            // already knows that head exists.
            // Lastly, note the difference between ':' and ':='. The former
            // creates a new handle while the later assigns a value to an
            // existing one.
            out : head.object; head := head.next; return out;
        },{
            // Otherwise, execute this code. The 'nothing' keyword
            // is similar to null or nullptr, but it can only be used
            // in the context of optional values which themselves enforce
            // handling of things that exist.
            return nothing;
        };
    };
},
// The 'hidden' keyword denotes that the constraints which follow
// are not accessible from outside the archetype.
hidden {
    head ? StackNode<Object>;
};

intStack : instance Stack<integer>;

// Here 'Clothing' inherits the archetype 'Item',
// it then further constrains legal values of a Clothing
// instance by modifying the constraint of the 'weight'
// member of 'Item'. Note that constraint specialization
// requires a constraint which is at most as large as the
// base archetype's original constraint on that member. To
// do otherwise is an error.
Clothing : Item { weight : number {0.1,20.0} }
{
    // If no constraint is given, the value could be
    // any legal value of Slot when a Clothing archetype
    // is instantiated.
    slot :  Slot;
};

// Here Shirt is derived from Clothing, with the added
// constraint that it's slot is always 'torso'.
Shirt : Clothing{  slot : 'torso' };

// The 'instance' keyword creates an actual object from
// an archetype. It is similar to the "new" keyword of
// C++, though Wish has a very different memory model.
// Note the similarity to C++14 initializers here, with
// the main differences being that constraint specializations
// use semicolons instead of commas and values are assigned to
// member by name. By omitting 'baseValue', we have given the
// runtime permission to substitute any value for the member
// that confroms to the constraints on it.
someShirt : instance Shirt{ weight : 0.5 };

Boot : Clothing{ slot : 'feet' };

// A collection of instances of the Clothing archetype.
// Note that this collection is actually a dictionary,
// mapping symbols to instances using the mapping operator '->'.
// We've also used the core localization feature, which uses
// a user-definable dictionary with virtualized members.
// Constraints use semicolons when they are listed like this;
// commas inside braces always denote a collection.
boots : instance Clothing... {
    'sabaton' -> Boot { name : localize@core['name_Sabaton']; weight : 0.2; },
    'slipper' -> Boot { name : localize@core['name_Slipper']; weight : 0.1; }
};

// Retrieving things from a dictionary uses the subscript
// operator '[]'.
boots['sabaton'].weight == 0.2;
boots['slipper'].weight == 0.1;

DamageType : symbol... {
    'bludgeon',
    'pierce',
    'slash'
};

Size : symbol... {
    'small',
    'medium',
    'large'
};


Actor : {
    hp : {0,500};
};

// Routines are basically functions. They have a call signature
// appended to their name and the constraint is the routine body.
// The syntax is a little different, but it is mostly familiar.
damage ( type : DamageType, damage : integer, target : Actor )
  : truth
{
    target.hp -= damage;
    // The 'return' keyword is available, but for uncomplicated things
    // you can omit it; the value of the last statement will be the
    // the return value.
    true;
    // Note we need a semicolon at the end of the routine body
    // because it is actually a constraint. Which means...
};

// You can in fact use anonymous routines to virtualize a member
// of an archetype. Notice the scope resoluton operator '@',
// deployed here to reference this object's 'size' member. We've also
// deployed the 'select' and 'in' keyword pair, which workslike
// a 'switch' statement. However here the possible values
// of the argument are mapped to other values using an
// anonymous dictionary.
Weapon : Item {
    size : Size;
    damage : number { select size@Item in { 'small' -> 5.0, 'medium' -> 10.0, 'large' -> 15.0, default -> 1.0 } };
    // Note this member has no constraint within the body of the archetype...
    bonusDamage : number;
};

// But we can define it out here instead using the scope
// resolution operator '@' as well. We've also added
// range qualifiers that exclude 1.0 and 100.0 from the range.
bonusDamage : number @ Weapon { >1.0, <100.0 };

// Which of course also means a routine can be a member of
// an archetype. By omitting the body we are promising that
// somewhere, someone implements it.
Spell : Equippable {
    cast( target : Actor ) : truth;
};

// Here we provide an implementation for the 'cast' member
// of the archetype 'Spell'.
cast( target : Actor ) : truth @ Spell { <do something>; };

// Omitting the body of an archetype means that until
// otherwise constrained, a FireSpell has the same range
// of values as a Spell.
FireSpell : Spell;

// Here we specialize the 'cast' member for objects of
// the archetype 'FireSpell'. All routine members of archetypes
// can be overridden in derived archetypes. We've also added
// a constraint specialzation to the argument target: it cannot
// have the 'player' type.
cast( target : Actor { &type != 'player' } ) : truth @ FireSpell { <do something else>; };

// Now we've deployed the 'derived_from' keyword, which
// does exactly what it says on the tin, though this example
// is somewhat nonsensical.
IceSpell : Spell {
    cast( target : Actor { derived_from(Goblin) } ) : truth { <do yet another thing>; };
}

// Here is an example of the implementation of the 'Quest' archetype.

Quest : <  > {
    instance()
};

Main : quest {
    instance( players : Player... )
      : quest
    {
        every p in players {
            p.level = 1;
            p.inventory << startingItems...;
            teleport@world( p, HomeTown );
        };
        return instance;
    };
    begin();
    end();
    release();
};

main : Main( {user} ).begin();

// ========= OLD NOTES BEYOND THIS POINT ==========

// Some bastardized BNF musings with regexes mixed in, probably wrong.

<identifier> -> <native-indentifier> | <user-identifier>
<native-indentifier> -> integer | number | quest | symbol

<statement> ->    <identifier>;
                | <identifier> : (<constraint>,)* <constraint>;
                | <identifier> : (<constraint>,)* <constraint> { <object-constraint>* };
<constraint> ->   <native-type>
                | <native-type> <literal-value>
                | <native-type> { <contraining-expression> }
                | <indetifier>

<constraining-expression> ->  <comparison-op> <value-expression>
<value-expression> -> <literal-value>

<literal-value> -> <symbol-literal> | <numerical-literal> | <real-literal>
<symbol-literal> -> '[azAZ_]+[azAZ09_]*'
<numerical-literal> -> [-]?[19][09]*
<real-literal> -> // This is hard to write without alot of thought


region HomeTown { <resource id?> };
region OtherTown { <resource id?> };

// The ellipsis indicates an unbounded and unordered
// collection of the indicated type.
symbol... Material {
    'steel',
    'leather',
    'linen',
};



symbol... Equipped {
    'equipped',
    'stored'
};

archetype Equippable {
    Equipped equipped;
    Slot slot;
};


// Archetypes are similar to classes, except they
// contain implicit instructions on what constitutes
// a valid instance of the archetype.
archetype Item {
    // A set builder notation defines the legal
    // ranges for attributes.
    // Numbers correspond to doubles: they can contain any real number.
    number weight | weight <- [0.1,100.0];
    // Integers correspond to 64bit signed integers.
    integer baseValue | baseValue <- [1, 100];
};

// You can define archetypes that inherit from other ones;
// in this way they work like classes.
archetype LeveledItem : Item {
    integer level | level <- [1,100];
};

archetype Clothing : Equippable, Item {
    Material material;
    Slot slot;
};

archetype Armor : Clothing {
    Clothing.material <- Material['steel', 'leather'];
};

archetype ArmorSet : LeveledItem {
    Armor cuirass | .slot == Slot['torso'];
    Armor helmet | .slot == Slot['head'];
    Armor cuisses | .slot == Slot['legs'];
    Armor gauntlets | .slot == Slot['hands'];
};

archetype Weapon : Equippable, Item {
    Equippable.slot == Slot['hands'];
};

archetype GoblinEquipment {
    ArmorSet armor | .level == 2;
    Weapon weapon | .level == 2;
}

Item... StartingItems {
    Clothing clothing | .material <- Material['linen'] and .baseValue == 0;
};

quest Main {

    Main( integer playerCount, Player[playerCount] players )
      : teleport( players..., HomeTown ),
        every( players... p ) { p.level = 1; p.inventory << startingItems...; }
    {
        every( players... p ) {
            // Stages with the tags 'main' and 'start' will start as
            // soon as their "requires" directives are fulfilled
            every( stage... s | 'main', 'start' in s.tags ) {
                await( begin s( p ) )
                  : capture( Choice c )
                {
                    // When Spawn finishes, we execute this
                    // block and can use anything in the capture
                    // statement
                }
            }
            await( begin Assassins( p ) )
              : capture( Choice c )
            {
                // Just like in ECMAScript, the await directive
                // returns immediately and we can launch other
                // stories.
            }
        }
    }


    
};

    quest Assassins ( Player player )
      : requires( player.level == 1 ),
        requires( player @ HomeTown )
    {
        

    };

    quest Spawn ( Player player )
      // Initializers fire before the stage actually starts
      : tags( 'start', 'main' )
        // requires directives delay the stage starting until they
        // are satisfied
        requires( player.level == 1 ),
        requires( player @ HomeTown )
    {
        
        return {
            // A whole JSON-like structure?
        }
    };
