using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheScript : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetBoolField(string field_name, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool GetBoolField(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetIntField(string field_name, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern int GetIntField(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetFloatField(string field_name, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetFloatField(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetDoubleField(string field_name, double value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern double GetDoubleField(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetStringField(string field_name, string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string GetStringField(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetGameObjectField(string field_name, TheGameObject value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject GetGameObjectField(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetVector3Field(string field_name, TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheVector3 GetVector3Field(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetQuaternionField(string field_name, TheQuaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheQuaternion GetQuaternionField(string field_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void CallFunction(string function_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern object CallFunctionArgs(string function_name, object[] args = null);
    }
}
