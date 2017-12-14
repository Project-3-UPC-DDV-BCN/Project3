﻿using System.Runtime.CompilerServices;
using System;

namespace TheEngine
{
    public class TheGameObject
    {
        public TheGameObject()
        {
            CreateNewGameObject(this);
        }

        public string name
        {
            get
            {
                return GetName();
            }
            set
            {
                SetName(value);
            }
        }

        public string tag
        {
            get
            {
                return GetTag();
            }
            set
            {
                SetTag(value);
            }
        }

        public string layer
        {
            get
            {
                return GetLayer();
            }
            set
            {
                SetLayer(value);
            }
        }


        public static TheGameObject Self
        {
            get
            {
                return GetSelf();
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void CreateNewGameObject(TheGameObject gameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetName(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern string GetName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsActive();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetTag(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern string GetTag();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetLayer(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern string GetLayer();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetStatic(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string IsStatic();

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheComponent GetComponent(Type type);

        public T GetComponent<T>() where T : TheComponent
        {
            return GetComponent(typeof(T)) as T;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheComponent AddComponent(Type type);

        public T AddComponent<T>() where T : TheComponent
        {
            return AddComponent(typeof(T)) as T;
        }
    }
}
