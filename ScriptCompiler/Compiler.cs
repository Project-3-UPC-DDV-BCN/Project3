using Microsoft.CSharp;
using System.CodeDom.Compiler;
using System.IO;
using System.Reflection;

namespace Compiler
{
    class Compiler
    {
        public static string Compile(string path, string name)
        {
            string result = "";

            CSharpCodeProvider cSharpCodeProvider = new CSharpCodeProvider();
            CompilerParameters parameters = new CompilerParameters();
            parameters.GenerateExecutable = false;
            parameters.OutputAssembly = name;
            parameters.IncludeDebugInformation = true;
            //parameters.GenerateInMemory = true;
            parameters.ReferencedAssemblies.Add(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + "\\System.dll");
            parameters.ReferencedAssemblies.Add(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + "\\TheEngine.dll");


            CompilerResults results = cSharpCodeProvider.CompileAssemblyFromFile(parameters, path);

            if (results.Errors.Count > 0)
            {
                int errors = 0;
                foreach (CompilerError CompErr in results.Errors)
                {
                    string error_type = "[Error]";
                    if(CompErr.IsWarning)
                    {
                        error_type = "[Warning]";
                    }
                    else
                    {
                        errors++;
                    }
                    result += error_type + CompErr.FileName + " at (" + CompErr.Line + "," + CompErr.Column + "), Error Number: " + CompErr.ErrorNumber + ", " + CompErr.ErrorText + "|";
                }
                if(errors == 0)
                {
                    result += results.CompiledAssembly.GetName().Name + ".cs Compiled!";
                }
            }
            else
            {
                result = results.CompiledAssembly.GetName().Name + ".cs Compiled!";
            }

            cSharpCodeProvider.Dispose();
            return result;
        }
    }
}