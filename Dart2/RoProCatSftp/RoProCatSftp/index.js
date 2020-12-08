console.log('Loading function');

const aws = require('aws-sdk');

const s3 = new aws.S3({ apiVersion: '2006-03-01' });


exports.handler = async (event, context) => {
    //console.log('Received event:', JSON.stringify(event, null, 2));

    // Get the object from the event and show its content type
    const bucket = event.Records[0].s3.bucket.name;
    const key = decodeURIComponent(event.Records[0].s3.object.key.replace(/\+/g, ' '));
    const params = {
        Bucket: bucket,
        Key: key,
    };
    try {
        const { ContentType } = await s3.getObject(params).promise();
        const data = await s3.getObject(params).promise();
        console.log('CONTENT TYPE:', ContentType);
        console.log('DATA', data);

        const config = {
            host: '10.134.100.122',
            port: '22',
            username: 'cat140802ftp',
            password: 'RoninProfessional1!'
        };
        const Client = require('ssh2-sftp-client');
        let sftp = new Client();
        let path = "/140802/cat/upload/";
        let fullPath = path += key;
        console.log('Full Path before connect:', fullPath);
        sftp.connect(config).then(() => {
            console.log('Full Path:', fullPath);
            sftp.put(data, fullPath);
        }).then(() => {
            console.log('listing:', fullPath);
            return sftp.list(path);
        }).then(() => {
            console.log('Fabout to end connect:', fullPath);
            return sftp.end();
        }).catch(err => {
            console.log('Full Path Error:', fullPath);
            console.error(err.message);
        });

        console.log('After Connect CONTENT TYPE:', ContentType);
        context.succeed(key);
        console.log('After succeed CONTENT TYPE:', ContentType);
        return ContentType;
    } catch (err) {
        console.log(err);
        const message = `Error getting object ${key} from bucket ${bucket}. Make sure they exist and your bucket is in the same region as this function.`;
        console.log(message);
        throw new Error(message);
    }
};

