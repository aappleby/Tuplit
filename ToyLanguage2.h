TreeNode = Struct(item : int, left : TreeNode?, right : TreeNode?);


TreeNode.ItemCheck = function(this : TreeNode) -> int {
  if (this.left && this.right) {
    result = this.item + left.ItemCheck() - right.ItemCheck();
  } else {
    result = this.item;
  }
}



int TreeNode_ItemCheck(TreeNode* this) {
  v0 = getField(this, 'left');
  v1 = getField(this, 'right');
  v2 = notNull(v0);
  v3 = notNull(v1);
  v4 = logicalAnd(v2, v3);
  jump_if_true v4, label1;
  v5 = getField(this, 'item');
  v6 = getField(v0, 'ItemCheck');
  v7 = call(v6, left);
  v8 = getField(v1, 'ItemCheck');
  v9 = call(v8, right);
  v10 = add(v5, v7);
  v11 = sub(v10, v9);
  return v11
label1:
  v5 = getField(this, 'item');
  return v5;
}


int TreeNode_ItemCheck(TreeNode* this) {
  v0 = fieldAt(arg[0], 1);
  v1 = fieldAt(arg[0], 2);
  v2 = notNull(v0);
  v3 = notNull(v1);
  v4 = and_bool_bool(v2, v3);
  jump_if_true v4, label1;
  v5 = fieldAt(arg[0], 0);
  v6 = TreeNode_ItemCheck
  v7 = call(v6, left);
  v8 = call(v6, right);
  v9 = add_int_int(v5, v7);
  v10 = sub_int_int(v10, v8);
  return v10
label1:
  v5 = getField(this, 'item');
  return v5;
}


SizedHash.methods = {
  size = function() : int;
  set = function(key : string, val : type(this.T));
  remove = function(key : string) : bool;
}


Class ( parent, constructors, destructor, methods );



vector = function(Type) : Type {

}


struct vector<T> {
  T * storage;
  int capacity;
  int size;
};

max = function (a, b) : * : {
  if (type(a) == any) {
    result = 10;
  } else {
    result = (a > b) ? a : b;
  }
}

print(max())

(nil)

function(a = nil, b = nil) : any {
  result = (a > b) ? a : b;
}

print(1 : 2 : 3)


//intmax := max(int,int):(int);

intmax = max : ((int,int), (int))

print(intmax);

function(a : int, b : int) : int {
  return (a > b) ? a : b;
}

boundmax1 := intmax(1, ?);

print(boundmax1)

function(a : int = 1, b : int = ?) : int {
  return (a > b) ? a : b;
}

// how to distinguish between evaulation and merge?
boundmax2 := boundmax1(2)

print(boundmax2)

function(a : int = 1, b : int = 2) : int {
  return (a > b) ? a : b;
}

boundmax2 : Function =
(
  args : * = (a : int = 1, b : int = 2)
  result : (int),
  context : nil,
  code : block = { result = (a > b) ? a : b; }
}


print boundmax2.args
(a : int = 1, b : int = 2)

print boundmax2.result
(int)

print type(boundmax2)
function

print name(boundmax2)
"boundmax2"




max = function
[
  function(a : int, b : int) : int { return a > b ? a : b; }
]




function : args : result : context : code




vector = function(T : *) : type {
  return {
    storage : primitive(T)*,
    capacity : int,
    size : int
  };
}

myvec = vector(int);

print(myvec)

(storage : int*, capacity: int, size : int)

somevec = myvec(null, 0, 0) 

print(somevec)

(storage : int* = null, capacity : int = 0, size : int = 0)

a = vector(int).create(10);


// token-izing operator?
foo := 2;
#"foo" := 2;
#("f" + "o" + "o") := 2;