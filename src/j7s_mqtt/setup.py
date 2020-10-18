from setuptools import setup

package_name = 'j7s_mqtt'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools', 'numpy', 'paho-mqtt'],
    zip_safe=True,
    maintainer='James Pace',
    maintainer_email='jpace121@gmail.com',
    description='TODO: Package description',
    license='Apache 2.0',
    entry_points={
        'console_scripts': [
            'sub=j7s_mqtt.sub:main',
            'pub=j7s_mqtt.pub:main',
        ],
    },
)
