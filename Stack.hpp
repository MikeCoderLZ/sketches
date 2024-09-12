namespace data {

  template< typename StoredType >
  class Stack {
    protected:
      class Node {
        public:
          Node();

          template< typename... Args >
          static Node* construct( Node* next, Args&&... args );

          static destruct( Node* );

        private:
          StoredType* next;
          StoredType  object;
      }

  }

} // namespace data