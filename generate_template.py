import os
from jinja2 import Template
import sys

projectName = sys.argv[1]
input_data = {
    'projectName': projectName,
}

# 创建目录
if not os.path.exists(projectName):
    os.makedirs(projectName)

# 读取模板文件
with open('CMakelists.template') as CMakelistTemplate:
    CMakelistTemplateContent = CMakelistTemplate.read()

with open('headerFile.template') as headerTemplate:
    headerTemplateContent = headerTemplate.read()

with open('codeFile.template') as codeTemplate:
    codeTemplateContent = codeTemplate.read()

with open('testFile.template') as testTemplate:
    testTemplateContent = testTemplate.read()

# 写入新文件
with open(projectName + '/CMakeLists.txt', 'w') as file:
    file.write(Template(CMakelistTemplateContent).render(input_data))

with open(projectName + '/' + projectName + '.h', 'w') as file:
    file.write(Template(headerTemplateContent).render(input_data))

with open(projectName + '/' + projectName + '.cpp', 'w') as file:
    file.write(Template(codeTemplateContent).render(input_data))

with open(projectName + '/' + projectName + 'Test.cpp', 'w') as file:
    file.write(Template(testTemplateContent).render(input_data))