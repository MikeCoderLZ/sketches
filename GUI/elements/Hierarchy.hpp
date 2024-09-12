#pragma once

namespace geni::elements {

    template< typename MemberType >
    class Hierarchy {
        public:

            class Member : public Hashable {
                private:
                    virtual
                    void hierarchyChanged( ParentRef& parent ) =0;
            };

            using ParentRef = WealRef<MemberType>;
            Hierarchy()
            { static_assert(std::derived_from<MemberType, Hierarchy::Member >); }
            virtual
            ~Hierarchy() = default;

            void addChild( Member& child )
            {
                mChildren.emplace( child.getHash(), WeakRef::to(child) );
                child.mParent = WeakRef::to(*this);
                child.hierarchyChanged( child.mParent );
            }
            void removeChild( Member& child )
            {
                auto maybeChild{ mChildren.find(child.getHash()) };
                if( maybeChild != mChildren.end() ) {
                    mChildren.remove( maybeChild );
                    child.mParent.clear();
                    child.hierarchyChanged( WeakRef<MemberType>::empty() );
                }
            }
        private:
            using ChildRef = WealRef<MemberType>;
            using ChildMap = std::map<size_t, ChildRef>;

            size_t      mHash;
            ParentRef   mParent;
            ChildMap    mChildren;
            
    };

} // namespace geni::elements

#define IsHierarchyOf(access, MemberType) access geni::elements::Hierarchy<MemberType>