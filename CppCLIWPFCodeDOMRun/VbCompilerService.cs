// Projekt: VBRoslynCompiler (Class Library, net8.0)
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.Text;
using Microsoft.CodeAnalysis.VisualBasic;
using System.IO;
using System.Reflection;

namespace CppCLIWPFCodeDOMRun;

public sealed record VbCompileResult(bool Success, string[] Diagnostics, byte[]? AssemblyImage);

public static class VbCompilerService
{
    public static VbCompileResult Compile(string code, string assemblyName, OutputKind outputKind)
    {
        var tree = VisualBasicSyntaxTree.ParseText(SourceText.From(code, System.Text.Encoding.UTF8));

        // Get all necessary runtime assemblies
        var assemblyPath = Path.GetDirectoryName(typeof(object).Assembly.Location)!;
        
        var references = new List<MetadataReference>
        {
            MetadataReference.CreateFromFile(typeof(object).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(Enumerable).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(Console).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(Microsoft.VisualBasic.Constants).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(Microsoft.VisualBasic.FileIO.FileSystem).Assembly.Location),
            MetadataReference.CreateFromFile(Path.Combine(assemblyPath, "System.Runtime.dll")),
            MetadataReference.CreateFromFile(Path.Combine(assemblyPath, "System.Console.dll")),
            MetadataReference.CreateFromFile(Path.Combine(assemblyPath, "System.Collections.dll")),
            MetadataReference.CreateFromFile(Path.Combine(assemblyPath, "System.Linq.dll")),
            MetadataReference.CreateFromFile(Path.Combine(assemblyPath, "netstandard.dll")),
        };

        // Add VB-specific global imports
        var globalImports = new[]
        {
            GlobalImport.Parse("System"),
            GlobalImport.Parse("System.Collections"),
            GlobalImport.Parse("System.Collections.Generic"),
            GlobalImport.Parse("System.Diagnostics"),
            GlobalImport.Parse("System.Linq"),
            GlobalImport.Parse("Microsoft.VisualBasic"),
        };

        var options = new VisualBasicCompilationOptions(outputKind)
            .WithEmbedVbCoreRuntime(true)
            .WithGlobalImports(globalImports)
            .WithRootNamespace("")
            .WithOptionExplicit(true)
            .WithMainTypeName(null);

        var compilation = VisualBasicCompilation.Create(
            assemblyName,
            new[] { tree },
            references,
            options
        );

        using var pe = new MemoryStream();
        var emit = compilation.Emit(pe);

        if (!emit.Success)
        {
            var diags = emit.Diagnostics
                .Where(d => d.Severity >= DiagnosticSeverity.Warning)
                .Select(d => d.ToString())
                .ToArray();

            return new VbCompileResult(false, diags, null);
        }

        return new VbCompileResult(true, Array.Empty<string>(), pe.ToArray());
    }
}
