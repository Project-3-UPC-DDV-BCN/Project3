﻿using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheRigidBody : TheComponent
    {

        public bool Kinematic
        {
            get
            {
                return IsKinematic(); 
            }

            set
            {
                SetKinematic(value);
            }
        }

        public bool Transform_go
        {
            get
            {
                return IsTransformGO(); 
            }

            set
            {
                SetKinematic(value); 
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern void SetKinematic(bool kinematic);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern bool IsKinematic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern void SetTransformGO(bool transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern bool IsTransformGO();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetLinearVelocity(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DisableCollider(int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DisableAllColliders();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetPosition(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(float x, float y, float z);
    }
}
