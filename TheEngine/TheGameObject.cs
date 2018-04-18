using System.Runtime.CompilerServices;
using System;

namespace TheEngine
{
    /// <summary>
    ///   <para>Base class for all enities in Scene</para>
    /// </summary>
    public class TheGameObject
    {
        /// <summary>
        ///   <para>Creates a new GameObject</para>
        /// </summary>
        public TheGameObject()
        {
            CreateNewGameObject(this);
        }

        /// <summary>
        ///   <para>GameObject name</para>
        /// </summary>
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

        /// <summary>
        ///   <para>GameObject tag</para>
        /// </summary>
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

        /// <summary>
        ///   <para>GameObject layer</para>
        /// </summary>
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

        /// <summary>
        ///   <para>GameObject this script is attached</para>
        /// </summary>
        public static TheGameObject Self
        {
            get
            {
                return GetSelf();
            }
        }

        /// <summary>
        ///   <para>Finds a GameObject</para>
        /// </summary>
        /// <param name="gameobject_name">Name of GameObject to find</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static TheGameObject Find(string gameobject_name);

        /// <summary>
        ///   <para>Return all GameObjects in scene</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static TheGameObject[] GetGameObjectsWithTag(string tag);

        /// <summary>
        ///   <para>Return all GameObjects in scene</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static TheGameObject[] GetGameObjectsMultipleTags(string[] tag);

        /// <summary>
        ///   <para>Return all childs in GameObject</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject[] GetAllChilds();

        /// <summary>
        ///   <para>Return all GameObjects in the given frustum</para>
        /// </summary>
        /// <param name="pos">Frustum pos</param>
        /// <param name="front">Front vector</param>
        /// <param name="up">Up Vector</param>
        /// <param name="nearPlaneDist">Near plane distance of Frustum</param>
        /// <param name="farPlaneDist">Far plane distance of Frustum</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static TheGameObject[] GetObjectsInFrustum(TheVector3 pos, TheVector3 front, TheVector3 up, float nearPlaneDist, float farPlaneDist);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void CreateNewGameObject(TheGameObject gameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetName(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern string GetName();

        /// <summary>
        ///   <para>Activates/Deactivates the GameObject</para>
        /// </summary>
        /// <param name="value">Active value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool value);

        /// <summary>
        ///   <para>Return active state</para>
        /// </summary>
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

        /// <summary>
        ///   <para>Sets GameObject static</para>
        /// </summary>
        /// <param name="value">Static value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetStatic(bool value);

        /// <summary>
        ///   <para>Return GameObject Static sate</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string IsStatic();

        /// <summary>
        ///   <para>Duplicates the given GameObject</para>
        /// </summary>
        /// <param name="original">GameObject to duplicate</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern TheGameObject Duplicate(TheGameObject original);

        /// <summary>
        ///   <para>Destroys the given GameObject</para>
        /// </summary>
        /// <param name="target">GameObject to destroy</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Destroy(TheGameObject target);

        /// <summary>
        ///   <para>Sets GameObject parent</para>
        /// </summary>
        /// <param name="new_parent">New GameObject parent</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetParent(TheGameObject new_parent);

        /// <summary>
        ///   <para>Return GameObject parent</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject GetParent();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern TheGameObject GetSelf();

        /// <summary>
        ///   <para>Return GameObject child by index</para>
        /// </summary>
        /// <param name="index">Child index</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject GetChild(int index);

        /// <summary>
        ///   <para>Return GameObject child by name</para>
        /// </summary>
        /// <param name="child_name">Child name</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject GetChild(string child_name);

        /// <summary>
        ///   <para>Return number of childs in GameObject</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern int GetChildCount();

        /// <summary>
        ///   <para>Destroys the given Component in GameObject</para>
        /// </summary>
        /// <param name="component">Component to destroy</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DestroyComponent(TheComponent component);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheComponent GetComponent(Type type, int index);

        /// <summary>
        ///   <para>Returns the component if the GameObject has one attached</para>
        /// </summary>
        public T GetComponent<T>(int index = 0) where T : TheComponent
        {
            return GetComponent(typeof(T), index) as T;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheScript GetScript(string script_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheComponent AddComponent(Type type);

        /// <summary>
        ///   <para>Adds the component to the GameObject</para>
        /// </summary>
        public T AddComponent<T>() where T : TheComponent
        {
            return AddComponent(typeof(T)) as T;
        }

        public override string ToString()
        {
            return "TheGameObject";
        }
    }
}
