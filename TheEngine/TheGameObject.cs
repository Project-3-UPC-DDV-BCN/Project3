﻿using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheGameObject
    {
        public TheGameObject()
        {
            CreateNewGameObject(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void CreateNewGameObject(TheGameObject gameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetName(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string GetName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsActive();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetTag(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string GetTag();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetLayer(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string GetLayerName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetStatic(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string IsStatic();

        public static TheGameObject Self
        {
            get
            {
                return GetSelf();
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern TheGameObject Duplicate(TheGameObject original);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Destroy(TheGameObject target);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetParent(TheGameObject new_parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern TheGameObject GetSelf();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject GetChild(int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject GetChild(string child_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern int GetChildCount();
    }
}
